#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <cstring>
#include <regex>
#include <iostream>

#include "Connection.h"
#include "Packet.h"
#include "Server.h"

Connection::Connection(int socket, sockaddr_in address, Server &server)
    : socket{socket},
      address{address},
      server{server},
      nickname{""},
      mode{Mode::Idle},
      game{nullptr}
{
    char ipString[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(address.sin_addr), ipString, INET_ADDRSTRLEN);

    this->ipString = ipString;
}

void Connection::run()
{
    server.getLogger().log("connection listening: " + ipString, Logger::Level::Success);

    char buffer[1024];
    std::string data;
    int corruptedPackets{0};

    while (!shouldStop() && !isClosed()) {

        ssize_t bytesRead = ::recv(socket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            // an error occurred

            if (errno == EAGAIN) {
                // client inactive
                auto now = std::chrono::steady_clock::now();
                auto inactive = now - lastRecvAt;

                if (inactive > inactiveTimeout) {
                    // inactive too long, probably dead
                    server.getLogger()
                        .log("connection: " + getId() + " inactive for too long - disconnecting",
                             Logger::Level::Warning);
                    return;
                }

                // send poke packet
                Packet pokePacket("poke");
                send(pokePacket);
            }
            else if (errno == EBADF || errno == EINVAL) {
                // socket shut down
                return;
            }
            else {
                // unrecoverable error
                server.getLogger()
                    .log("ERROR: error while receiving from the connection: " + getId() + ": "
                             + std::string{strerror(errno)}, Logger::Level::Error);
                return;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            server.getLogger()
                .log("player orderly disconnected: " + getId());
            return;
        }

        lastRecvAt = std::chrono::steady_clock::now();
        server.getStats().addBytesReceived(static_cast<uint64_t>(bytesRead));

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {
            bool endMessage{false};

            for (int j = 0; j < Packet::TERMINATOR.size() - 1; ++j) {
                if (buffer[i + j] != Packet::TERMINATOR[j]) {
                    endMessage = false;
                    break;
                }
                endMessage = true;
            }

            // search for the termination symbol
            if (endMessage) {
                // if found, the whole message was received

                //skip termination symbols
                i += Packet::TERMINATOR.size() - 1;

                try {
                    Packet packet;
                    packet.parse(data);

                    server.getLogger().logCommunication(packet, true, getId());
                    handlePacket(packet);

                    corruptedPackets = 0;
                    server.getStats().addPacketsReceived(1);
                }
                catch (PacketException &exception) {
                    server.getStats().addPacketsDropped(1);
                    corruptedPackets++;
                }
                catch (...) {
                    server.getLogger()
                        .log("ERROR: unrecoverable error on connection: " + getId() + " - disconnecting",
                             Logger::Level::Error);
                    stop(false);
                    break;
                }

                data.clear();
            }
            else {
                data += buffer[i];
            }
        }


        if (data.size() > Packet::MAX_SIZE) {
            // buffered data exceeds the normal message length
            // message is probably corrupted
            server.getStats().addBytesDropped(data.size());
            data.clear();
            corruptedPackets++;
        }

        if (corruptedPackets > CORRUPTED_PACKETS_LIMIT) {
            // connection probably corrupted
            server.getLogger()
                .log("ERROR: too much corrupted data receive from the connection: " + getId() + " - disconnecting",
                     Logger::Level::Error);
            break;
        }
    }
}

void Connection::send(Packet &packet)
{
    std::string contents = packet.serialize();

    while (!isClosed()) {
        ssize_t sentBytes = ::send(socket, contents.c_str(), contents.length(), 0);

        if (sentBytes >= 0) {
            server.getStats().addPacketsSent(1);
            server.getStats().addBytesSent(static_cast<uint64_t>(sentBytes));
            server.getLogger().logCommunication(packet, false, getId());
            break;
        }

        // an error occurred
        if (errno == EAGAIN) {
            // internal sending buffer full
            auto now = std::chrono::steady_clock::now();
            auto inactive = now - lastSendAt;

            if (inactive < inactiveTimeout) {
                // inactive time limit not exceeded
                // try send again
                continue;
            }
        }
        else if (errno == EBADF || errno == EINVAL) {
            // socket shut down
        }
        else {
            // unrecoverable error
            stop(false);
        }

        break;
    }
}

bool Connection::isClosed()
{
    return socket == -1;
}

bool Connection::isIdentified()
{
    return !nickname.empty();
}

std::string Connection::getId()
{
    return nickname.empty() ? ipString : nickname;
}

void Connection::setMode(Connection::Mode mode)
{
    timeval recvTimeout{};
    timeval sendTimeout{};

    switch (mode) {
    case Mode::Idle:
        recvTimeout = RECV_TIMEOUT_IDLE;
        sendTimeout = SEND_TIMEOUT_IDLE;
        inactiveTimeout = INACTIVE_TIMEOUT_IDLE;
        break;
    case Mode::Busy:
        recvTimeout = RECV_TIMEOUT_BUSY;
        sendTimeout = SEND_TIMEOUT_BUSY;
        inactiveTimeout = INACTIVE_TIMEOUT_BUSY;
        break;
    default:
        throw ConnectionException("can not set mode " + std::to_string(static_cast<int>(mode)));
    }

    this->mode = mode;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const void *>(&recvTimeout), sizeof(recvTimeout));
    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const void *>(&sendTimeout), sizeof(sendTimeout));
}

bool Connection::stop(bool wait)
{
    ::shutdown(socket, SHUT_RDWR);
    return Thread::stop(wait);
}

void Connection::before()
{
    setMode(Mode::Idle);
}

void Connection::after()
{
    ::close(socket);
    socket = -1;
    server.getLogger().log("connection closed: " + ipString, Logger::Level::Warning);
}

void Connection::handlePacket(Packet packet)
{
    auto typeHandler = handlers.find(packet.getType());

    if (typeHandler == handlers.end()) {
        throw UnknownPacketException("unknown packet type: [" + packet.getType() + "]");
    }

    Handler handler = typeHandler->second;

    (this->*handler)(packet);
}

void Connection::handleLogin(Packet packet)
{
    if (isIdentified()) {
        throw NonContextualPacketException{"already logged"};
    }

    auto items = packet.getItems();

    if (items.size() != 1 ) {
        throw MalformedPacketException{"login packet must have only 1 argument: nickname"};
    }

    std::regex nicknameRegex("[a-zA-Z0-9]{3,16}");
    std::string nickname = *items.begin();

    Packet response{};

    if (std::regex_match(nickname, nicknameRegex)) {
        try {
            server.addPlayer(nickname, this);
            this->nickname = nickname;
            response.setType("login_ok");
        }
        catch (ServerException &exception) {
            response.setType("login_failed");
            response.addItem("exists");
        }
    }
    else {
        response.setType("login_failed");
        response.addItem("format");
    }

    send(response);
}

void Connection::handleJoinRandomGame(Packet packet)
{
    if (game) {
        throw NonContextualPacketException{"already in a game"};
    }

    if (!isIdentified()) {
        throw NonContextualPacketException{"not logged"};
    }


    // TODO: handle join random game
}

void Connection::handleJoinPrivateGame(Packet packet)
{
    if (game) {
        throw NonContextualPacketException{"already in a game"};
    }

    if (!isIdentified()) {
        throw NonContextualPacketException{"not logged"};
    }

    // TODO: handle join private game
}

void Connection::handleUpdateState(Packet packet)
{
    if (!game) {
        throw NonContextualPacketException{"not in a game"};
    }

    try {
        PlayerState state{packet.getItems()};
        game->updatePlayerState(nickname, state);

        std::list<std::string> items;
        state.itemize(items);

        Packet forOpponent{"update_opponent", items};
    }
    catch (GameTypeException &exception) {
        throw MalformedPacketException{exception.what()};
    }
    catch (GamePlayException &exception) {
        throw InvalidPacketException{exception.what()};
    }
}

void Connection::handleBallHit(Packet packet)
{
    if (!game) {
        throw NonContextualPacketException{"not in a game"};
    }

    if (packet.getItems().size() != PlayerState::ITEMS_COUNT + BallState::ITEMS_COUNT) {
        throw MalformedPacketException{"too few items in the packet"};
    }

    try {
        // extract items from packet
        std::list<std::string> packetItems = packet.getItems();

        auto it = packetItems.begin();
        std::advance(it, PlayerState::ITEMS_COUNT - 1);

        std::list<std::string> playerItems{packetItems.begin(), it};
        std::list<std::string> ballItems{packetItems.begin(), packetItems.end()};


        // create player and ball states from items and update game
        PlayerState playerState{playerItems};
        game->updatePlayerState(nickname, playerState);

        BallState ballState{ballItems};
        game->ballHit(nickname, ballState);


        // send responses
        std::list<std::string> items;
        Connection &opponent = server.getConnection(game->getOpponentNickname(nickname));

        if (game->getState() == GameState::NewRound) {
            game->itemize(items);
            Packet response{"new_round", items};

            send(response);
            opponent.send(response);
        }
        else {
            ballState.itemize(items);
            Packet forOpponent{"ball_hit", items};
            opponent.send(forOpponent);
        }
    }
    catch (GameTypeException &exception) {
        throw MalformedPacketException{exception.what()};
    }
    catch (GamePlayException &exception) {
        throw InvalidPacketException{exception.what()};
    }
}

void Connection::handleLeaveGame(Packet packet)
{
    // TODO: handle leave
}

void Connection::handlePoke(Packet packet)
{
    Packet response{"poke_back"};
    send(response);
}

std::string Connection::getNickname()
{
    return nickname;
}

void Connection::handlePokeBack(Packet packet)
{}


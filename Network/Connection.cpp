#include <arpa/inet.h>
#include <unistd.h>

#include <regex>

#include "Connection.h"
#include "Packet.h"
#include "Server.h"

Connection::Connection(Uid uid, int socket, sockaddr_in address, Server &server)
    : uid{uid},
      socket{socket},
      address{address},
      server{server},
      nickname{""},
      mode{ConnectionMode::Idle},
      game{nullptr}
{
    char ipString[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(address.sin_addr), ipString, INET_ADDRSTRLEN);

    ip = ipString;
}

Uid Connection::getUid()
{
    return uid;
}

std::string Connection::getUidStr()
{
    return std::to_string(uid);
}

std::string Connection::getNickname()
{
    return nickname;
}

bool Connection::isIdentified()
{
    return !nickname.empty();
}

bool Connection::isClosed()
{
    return socket == -1;
}

void Connection::setMode(ConnectionMode mode)
{
    timeval recvTimeout{};

    switch (mode) {
    case ConnectionMode::Idle: {
        recvTimeout = RECV_TIMEOUT_IDLE;
        inactiveTimeout = INACTIVE_TIMEOUT_IDLE;
        break;
    }
    case ConnectionMode::Busy: {
        recvTimeout = RECV_TIMEOUT_BUSY;
        inactiveTimeout = INACTIVE_TIMEOUT_BUSY;
        break;
    }
    default: {
        throw ConnectionException("can not set connection mode " + std::to_string(static_cast<int>(mode)));
    }
    }

    this->mode = mode;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const void *>(&recvTimeout), sizeof(recvTimeout));
}

void Connection::send(Packet &packet)
{
    std::string contents = packet.serialize();

    ssize_t sentBytes = ::send(socket, contents.c_str(), contents.length(), 0);

    if (sentBytes == 0) {
        // success
        server.getStats().addPacketsSent(1);
        server.getStats().addBytesSent(static_cast<uint64_t>(sentBytes));
        server.getLogger().logCommunication(packet, false, getUid());
    }
}

void Connection::before()
{
    if (isClosed()) {
        throw ConnectionException("invalid socket");
    }

    setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const void *>(&SEND_TIMEOUT), sizeof(SEND_TIMEOUT));
    setMode(ConnectionMode::Idle);
}

void Connection::run()
{
    server.getLogger().log(Text::justifyL(getUidStr(), 6) + " - listening", Logger::Level::Success);

    char buffer[1024];
    std::string data;
    int corruptedPackets{0};

    while (!shouldStop() && !isClosed()) {

        // receive available bytes
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
                        .log(Text::justifyL(getUidStr(), 6) + " - inactive for too long - disconnecting",
                             Logger::Level::Warning);
                    return;
                }

                // send poke packet
                Packet pokePacket("poke");
                send(pokePacket);
                continue;
            }
            else if (errno == EBADF || errno == EINVAL) {
                // socket shut down
                return;
            }
            else {
                // unrecoverable error
                server.getLogger()
                    .log(Text::justifyL(getUidStr(), 6) + " - error while receiving: " + std::string{strerror(errno)},
                         Logger::Level::Error);
                return;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            server.getLogger()
                .log(Text::justifyL(getUidStr(), 6) + " - orderly disconnected");
            return;
        }

        lastRecvAt = std::chrono::steady_clock::now();
        server.getStats().addBytesReceived(static_cast<uint64_t>(bytesRead));

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {
            bool endMessage{false};

            // search for the termination symbol
            for (int j = 0; j < Packet::TERMINATOR.size() - 1; ++j) {
                if (buffer[i + j] != Packet::TERMINATOR[j]) {
                    endMessage = false;
                    break;
                }
                endMessage = true;
            }

            if (endMessage) {
                // if found, the whole message was received

                //skip termination symbols
                i += Packet::TERMINATOR.size() - 1;

                // handle received message
                try {
                    Packet packet = Packet::parse(data);

                    server.getLogger().logCommunication(packet, true, getUid());
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
                        .log(Text::justifyL(getUidStr(), 6) + " - unrecoverable error - disconnecting: ",
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
                .log(Text::justifyL(getUidStr(), 6) + " - too much corrupted data received - disconnecting",
                     Logger::Level::Error);
            break;
        }
    }
}

bool Connection::stop(bool wait)
{
    // shutdown the socket to break the blocking recv() call
    ::shutdown(socket, SHUT_RDWR);
    return Thread::stop(wait);
}

void Connection::after()
{
    ::close(socket);
    socket = -1;
    server.getLogger().log(Text::justifyL(getUidStr(), 6) + " - connection closed", Logger::Level::Warning);
}

void Connection::handlePacket(Packet packet)
{
    auto typeHandler = PACKET_HANDLERS.find(packet.getType());

    if (typeHandler == PACKET_HANDLERS.end()) {
        throw UnknownPacketException("unknown packet type: [" + packet.getType() + "]");
    }

    PacketHandler handler = typeHandler->second;

    (this->*handler)(packet);
}

void Connection::handlePoke(Packet packet)
{
    Packet response{"poke_back"};
    send(response);
}

void Connection::handlePokeBack(Packet packet)
{}

void Connection::handleLogin(Packet packet)
{
    if (isIdentified()) {
        throw NonContextualPacketException{"already logged"};
    }

    auto items = packet.getItems();

    if (items.size() != 1) {
        throw MalformedPacketException{"login packet must have only 1 argument: nickname"};
    }

    std::regex nicknameRegex("[a-zA-Z0-9]{3,16}");
    std::string nickname = *items.begin();

    Packet response{};

    if (std::regex_match(nickname, nicknameRegex)) {
        try {
            server.addNickname(nickname);
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
//    if (game) {
//        throw NonContextualPacketException{"already in a game"};
//    }
//
//    if (!isIdentified()) {
//        throw NonContextualPacketException{"not logged"};
//    }


    // TODO: handle join random game
}

void Connection::handleJoinPrivateGame(Packet packet)
{
//    if (game) {
//        throw NonContextualPacketException{"already in a game"};
//    }
//
//    if (!isIdentified()) {
//        throw NonContextualPacketException{"not logged"};
//    }

    // TODO: handle join private game
}

void Connection::handleCreatePrivateGame(Packet packet)
{
    // TODO: handle crate private game
}

void Connection::handleLeaveGame(Packet packet)
{
    // TODO: handle leave
}

void Connection::handleGetTime(Packet packet)
{
    // TODO: handle get time
}

void Connection::handleReady(Packet packet)
{
    // TODO: handle ready
}

void Connection::handleUpdateState(Packet packet)
{
//    if (!game) {
//        throw NonContextualPacketException{"not in a game"};
//    }
//
//    try {
//        PlayerState state{packet.getItems()};
//        game->updatePlayerState(nickname, state);
//
//        std::list<std::string> items;
//        state.itemize(items);
//
//        Packet forOpponent{"update_opponent", items};
//    }
//    catch (GameTypeException &exception) {
//        throw MalformedPacketException{exception.what()};
//    }
//    catch (GamePlayException &exception) {
//        throw InvalidPacketException{exception.what()};
//    }
    // TODO: handle update state
}

void Connection::handleBallHit(Packet packet)
{
//    if (!game) {
//        throw NonContextualPacketException{"not in a game"};
//    }
//
//    if (packet.getItems().size() != PlayerState::ITEMS_COUNT + BallState::ITEMS_COUNT) {
//        throw MalformedPacketException{"too few items in the packet"};
//    }
//
//    try {
//        // extract items from packet
//        std::list<std::string> packetItems = packet.getItems();
//
//        auto it = packetItems.begin();
//        std::advance(it, PlayerState::ITEMS_COUNT - 1);
//
//        std::list<std::string> playerItems{packetItems.begin(), it};
//        std::list<std::string> ballItems{packetItems.begin(), packetItems.end()};
//
//
//        // create player and ball states from items and update game
//        PlayerState playerState{playerItems};
//        game->updatePlayerState(nickname, playerState);
//
//        BallState ballState{ballItems};
//        game->ballHit(nickname, ballState);
//
//
//        // send responses
//        std::list<std::string> items;
//        Connection &opponent = server.getConnection(game->getOpponentNickname(nickname));
//
//        if (game->getState() == GameState::NewRound) {
//            game->itemize(items);
//            Packet response{"new_round", items};
//
//            send(response);
//            opponent.send(response);
//        }
//        else if (game->getState() == GameState::End) {
//            game->itemize(items);
//            Packet response{"game_end", items};
//
//            send(response);
//            opponent.send(response);
//        }
//        else {
//            ballState.itemize(items);
//            Packet forOpponent{"ball_hit", items};
//            opponent.send(forOpponent);
//        }
//    }
//    catch (GameTypeException &exception) {
//        throw MalformedPacketException{exception.what()};
//    }
//    catch (GamePlayException &exception) {
//        throw InvalidPacketException{exception.what()};
//    }
    // TODO: handle ball hit
}

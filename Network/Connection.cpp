#include <unistd.h>

#include <thread>
#include <arpa/inet.h>
#include <cstring>

#include "Connection.h"
#include "Packet.h"
#include "Server.h"

Connection::Connection(int socket, sockaddr_in address, Server &server)
    : socket{socket},
      address{address},
      server{server},
      identified{false},
      mode{Mode::Idle}
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
        server.getStats().addBytesReceived(bytesRead);

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {
            data += buffer[i];

            // search for the termination symbol
            if (buffer[i] == Packet::TERMINATOR) {
                // if find, the whole message was received

                try {
                    Packet packet;
                    packet.parse(data);
                    handlePacket(packet);

                    corruptedPackets = 0;

                    server.getStats().addPacketsReceived(1);

                    server.getLogger()
                        .logCommunication(packet, true, getId());
                }
                catch (PacketException &exception) {
                    server.getStats().addPacketsDropped(1);
                    corruptedPackets++;
                }

                data.clear();
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
                .log("ERROR: too much corrupted data receive from the connection: " + getId() + " - disconnecting", Logger::Level::Error);
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
            server.getStats().addBytesSent(sentBytes);
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

std::string Connection::getId()
{
    return ipString;
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
    // TODO: implement packet handling
}


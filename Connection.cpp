#include <unistd.h>

#include <thread>

#include "Connection.h"
#include "Packet.h"
#include "Server.h"

Connection::Connection(int socket, sockaddr_in address, Server &server)
    : socket{socket},
      address{address},
      server{server},
      identified{false},
      mode{Mode::Idle}
{}

void Connection::run()
{
    char buffer[1024];
    std::string data;
    int corruptedPackets{0};

    setMode(Mode::Idle);


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
                    break;
                }
            }
            else {
                // unrecoverable error
                break;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            break;
        }

        lastRecvAt = std::chrono::steady_clock::now();

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {
            data += buffer[i];

            // search for the termination symbol
            if (buffer[i] == Packet::TERMINATOR) {
                // if find, the whole message was received

                try {
                    Packet packet{data};
                    handlePacket(packet);
                    corruptedPackets = 0;
                }
                catch (PacketException &exception) {
                    corruptedPackets++;
                }

                data.clear();
            }
        }


        if (data.size() > Packet::MAX_SIZE) {
            // buffered data exceeds the normal message length
            // message is probably corrupted
            data.clear();
            corruptedPackets++;
        }

        if (corruptedPackets > CORRUPTED_PACKETS_LIMIT) {
            // connection probably corrupted
            break;
        }
    }

    closeSocket();
}

void Connection::send(Packet &packet)
{
    std::string contents = packet.serialize();

    while (!isClosed()) {
        ssize_t sentBytes = ::send(socket, contents.c_str(), contents.length(), 0);

        if (sentBytes >= 0) {
            break;
        }

        // an error occurred
        if (errno == EAGAIN) {
            // internal sending buffer full
            auto now = std::chrono::steady_clock::now();
            auto inactive = now - lastSendAt;

            if (inactive > inactiveTimeout) {
                // inactive too long, probably dead
                closeSocket();
                break;
            }
        }
        else {
            // unrecoverable error
            closeSocket();
            break;
        }
    }
}

bool Connection::isClosed()
{
    return socket == -1;
}

void Connection::closeSocket()
{
    ::close(socket);
    socket = -1;
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

void Connection::handlePacket(Packet packet)
{
    // TODO: implement packet handling
}


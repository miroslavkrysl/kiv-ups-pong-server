#include <unistd.h>
#include <arpa/inet.h>

#include <cstring>

#include "Connection.h"
#include "../App.h"
#include "../Util/Logger.h"
#include "../Util/Text.h"
#include "../Exceptions.h"

void Connection::setMode(Connection::Mode mode)
{
    timeval recvTimeout{};

    switch (mode) {
    case Mode::Idle: {
        recvTimeout = RECV_TIMEOUT_IDLE;
        inactiveTimeout = INACTIVE_TIMEOUT_IDLE;
        break;
    }
    case Mode::Busy: {
        recvTimeout = RECV_TIMEOUT_BUSY;
        inactiveTimeout = INACTIVE_TIMEOUT_BUSY;
        break;
    }
    }


    int returnValue = setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const void *>(&recvTimeout), sizeof(recvTimeout));

    if (returnValue == -1) {
        throw ConnectionException(
            "can not set the socket option for receive timeout: " + std::string{std::strerror(errno)});
    }

    this->mode = mode;
}

Connection::Connection(App &app, Uid uid, int socket, sockaddr_in address)
    : app(app),
      uid(uid),
      socket(socket),
      address(address),
      port(ntohs(address.sin_port))
{
    if (socket < 0) {
        throw ConnectionException("invalid connection socket");
    }

    if (uid < 0) {
        throw ConnectionException("invalid connection uid");
    }

    char ipChars[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(address.sin_addr), ipChars, INET_ADDRSTRLEN);

    this->ip = ipChars;

    setMode(Mode::Idle);
    int returnValue = setsockopt(
        socket,
        SOL_SOCKET,
        SO_SNDTIMEO,
        reinterpret_cast<const void *>(&SEND_TIMEOUT),
        sizeof(SEND_TIMEOUT));

    if (returnValue == -1) {
        throw ConnectionException(
            "can not set the socket option for send timeout: " + std::string{std::strerror(errno)});
    }
}

Uid Connection::getUid() const
{
    return uid;
}

Port Connection::getPort() const
{
    return port;
}

std::string Connection::getIp() const
{
    return ip;
}

const sockaddr_in &Connection::getAdress() const
{
    return address;
}

Connection::Mode Connection::getMode() const
{
    return mode;
}

void Connection::send(const Packet &packet) const
{
    std::string contents = packet.serialize();

    ssize_t sentBytes = ::send(socket, contents.c_str(), contents.length(), 0);

    if (sentBytes != 0) {
        // success
        app.getStats().addPacketsSent(1);
        app.getStats().addBytesSent(static_cast<uint64_t>(sentBytes));
        app.getLogger().logCommunication(packet, false, getUid());
    }
}

void Connection::before()
{
    if (socket == -1) {
        throw ConnectionException("invalid connection socket");
    }
}

void Connection::run()
{
    app.getLogger().log(std::to_string(uid) + " - listening", Logger::Level::Success);

    char buffer[1024];
    std::string data;
    int corruptedPackets{0};

    while (!shouldStop()) {

        // receive available bytes
        ssize_t bytesRead = ::recv(socket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            // an error occurred

            if (errno == EAGAIN) {
                // client inactive
                auto now = std::chrono::steady_clock::now();
                auto inactive = now - lastActiveAt;

                if (inactive > inactiveTimeout) {
                    // inactive too long, probably dead
                    app.getLogger()
                        .log(std::to_string(uid) + " - inactive for too long - disconnecting",
                             Logger::Level::Warning);
                    return;
                }

                // send poke packet
                send(Packet{"poke"});

                continue;
            }
            else if (errno == EBADF || errno == EINVAL) {
                // socket shut down
                return;
            }
            else {
                // unrecoverable error
                app.getLogger()
                    .log(std::to_string(uid) + " - error while receiving: " + std::string{strerror(errno)},
                         Logger::Level::Error);
                return;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            app.getLogger()
                .log(std::to_string(uid) + " - orderly disconnected");
            return;
        }

        lastActiveAt = std::chrono::steady_clock::now();
        app.getStats().addBytesReceived(static_cast<uint64_t>(bytesRead));

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
                Packet packet = Packet::parse(data);

                app.getLogger().logCommunication(packet, true, getUid());

                try {
                    // TODO use packet handler
//                    app.getPacketHandler()->handleReceiving(packet);
                    app.getLogger().log("received: " + packet.toLog());
                    corruptedPackets = 0;
                    app.getStats().addPacketsReceived(1);
                }
                catch (MalformedPacketException &exception) {
                    corruptedPackets++;
                    app.getStats().addPacketsDropped(1);
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
            app.getStats().addBytesDropped(data.size());
            data.clear();
            corruptedPackets++;
        }

        if (corruptedPackets > CORRUPTED_PACKETS_LIMIT) {
            // connection probably corrupted
            app.getLogger()
                .log(std::to_string(uid) + " - too much corrupted data received - disconnecting",
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
    app.getLogger().log(std::to_string(uid) + " - connection closed", Logger::Level::Warning);
}

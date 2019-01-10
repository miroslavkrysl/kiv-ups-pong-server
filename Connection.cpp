#include <unistd.h>

#include <thread>

#include "Connection.h"
#include "Packet.h"
#include "Server.h"

Connection::Connection(int socket, sockaddr_in address, uint32_t uid, Server &server)
    : socket{socket},
      address{address},
      uid{uid},
      disconnected{false},
      identified{false},
      CORRUPTED_PACKETS_LIMIT{5},
      RECONNECTION_TIME_LIMIT{30},
      RECV_TIMEOUT{10, 0},
      server{server}
{
}

void Connection::run()
{
    char buffer[1024];
    std::string data;
    int corruptedPackets{0};
    auto disconnectedFrom = std::chrono::steady_clock::now();

    // set socket recv timeout
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const void *>(&RECV_TIMEOUT), sizeof(RECV_TIMEOUT));

    while (!shouldStop()) {

        ssize_t bytesRead = recv(socket, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            // an error occured

            if (errno == EAGAIN
                || errno == ECONNABORTED
                || errno == ECONNRESET) {
                // network errors, try to reconnect

                if (!disconnected) {
                    disconnected = true;
                    disconnectedFrom = std::chrono::steady_clock::now();
                }

                auto now = std::chrono::steady_clock::now();
                auto disconnectedTime = now - disconnectedFrom;

                if (disconnectedTime > RECONNECTION_TIME_LIMIT) {
                    // disconnected too long
                    closeSocket();
                    break;
                }

                std::this_thread::sleep_for(std::chrono::seconds{1});
            }
            else {
                // unrecoverable error
                closeSocket();
                break;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            closeSocket();
            break;
        }

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {
            data += buffer[i];

            // search for the termination symbol
            if (buffer[i] == Message::TERMINATION_SYMBOL) {
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


        if (data.size() > Message::MAX_SIZE) {
            // buffered data exceeds the normal message length
            // message is probably corrupted
            data.clear();
            corruptedPackets++;
        }

        if (corruptedPackets > CORRUPTED_PACKETS_LIMIT) {
            // connection probably corrupted
            closeSocket();
            break;
        }

        disconnected = false;
    }
}

void Connection::send(Packet &packet)
{
    if (isDisconnected() || isClosed()) {
        throw ConnectionException("cant send data to the connection");
    }

    std::string contents = packet.serialize();
    ::send(socket, contents.c_str(), contents.length(), 0);
}

bool Connection::isDisconnected()
{
    return disconnected;
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

void Connection::handlePacket(Packet packet)
{
    // TODO: implement packet handling
}


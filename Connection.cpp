#include <unistd.h>

#include <thread>

#include "Connection.h"
#include "Server.h"

Connection::Connection(int socket, sockaddr_in address, uint32_t uid, Server *server)
    : socket_{socket},
      address_{address},
      uid_{uid},
      server_{server},
      disconnected_{false},
      identified_{false},
      CORRUPTED_MESSAGES_LIMIT_{5},
      RECONNECTION_TIME_LIMIT_{30},
      RECV_TIMEOUT{10, 0}
{
}

void Connection::run()
{
    char buffer[1024];
    std::string data;
    int corruptedMessages{0};
    auto disconnectedFrom = std::chrono::steady_clock::now();

    // set socket recv timeout
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const void *>(&RECV_TIMEOUT), sizeof(RECV_TIMEOUT));

    while (!shouldStop()) {

        ssize_t bytesRead = recv(socket_, buffer, sizeof(buffer), 0);

        if (bytesRead == -1) {
            // an error occured

            if (errno == EAGAIN
                || errno == ECONNABORTED
                || errno == ECONNRESET) {
                // network errors, try to reconnect

                if (!disconnected_) {
                    disconnected_ = true;
                    disconnectedFrom = std::chrono::steady_clock::now();
                }

                auto now = std::chrono::steady_clock::now();
                auto disconnectedTime = now - disconnectedFrom;

                if (disconnectedTime > RECONNECTION_TIME_LIMIT_) {
                    // disconnected too long
                    closeSocket_();
                    break;
                }

                std::this_thread::sleep_for(std::chrono::seconds{1});
            }
            else {
                // unrecoverable error
                closeSocket_();
                break;
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            closeSocket_();
            break;
        }

        // process the received data
        for (int i = 0; i < bytesRead; ++i) {

            // search for the termination symbol
            if (buffer[i] == Message::TERMINATION_SYMBOL) {
                // if find, the whole message was received

                try {
                    Message message = Message::parse(data);
                    server_.handleMessage(message);
                    corruptedMessages = 0;
                }
                catch (MessageException &exception) {
                    corruptedMessages++;
                }

                data.clear();
            }
            else {
                data += buffer[i];
            }
        }


        if (data.size() > Message::MAX_SIZE) {
            // buffered data exceeds the normal message length
            // message is probably corrupted
            data.clear();
            corruptedMessages++;
        }

        if (corruptedMessages > CORRUPTED_MESSAGES_LIMIT_) {
            // connection probably corrupted
            closeSocket_();
            break;
        }

        disconnected_ = false;
    }
}

void Connection::send(const Message &message)
{
    if (disconnected() || closed()) {
        throw ConnectionException("cant send data to the connection");
    }

    std::string contents = message.serialize();
    ::send(socket_, contents.c_str(), contents.length(), 0);
}

bool Connection::disconnected()
{
    return disconnected_;
}

bool Connection::closed()
{
    return socket_ == -1;
}

void Connection::closeSocket_()
{
    ::close(socket_);
    socket_ = -1;
}

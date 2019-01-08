#pragma once

#include <netinet/in.h>

#include "Message.h"
#include "Thread.h"
#include "Server.h"

class Server;

class Connection: public Thread
{
    int socket_;
    sockaddr_in address_;
    uint32_t uid_;
    Server *server_;

    const int CORRUPTED_MESSAGES_LIMIT_;
    const std::chrono::duration<int> RECONNECTION_TIME_LIMIT_;
    const timeval RECV_TIMEOUT;

    bool disconnected_;
    bool identified_;

    void closeSocket_();

public:
    Connection(int socket, sockaddr_in address, uint32_t uid, Server *server);
    Connection(const Connection &connection) = delete;

    void send(const Message &message);
    void run() override;

    bool disconnected();
    bool closed();
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

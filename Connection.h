#pragma once

#include <netinet/in.h>

#include "Message.h"
#include "Thread.h"
#include "Server.h"

class Server;

class Connection: public Thread
{
    int socket;
    sockaddr_in address;
    uint32_t uid;
    Server &server;

    const int CORRUPTED_MESSAGES_LIMIT;
    const std::chrono::duration<int> RECONNECTION_TIME_LIMIT;
    const timeval RECV_TIMEOUT;

    bool disconnected;
    bool identified;

    void closeSocket();

public:
    Connection(int socket, sockaddr_in address, uint32_t uid, Server &server);
    Connection(const Connection &connection) = delete;

    void send(const Message &message);
    void run() override;

    bool isDisconnected();
    bool isClosed();
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

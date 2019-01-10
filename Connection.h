#pragma once

#include <netinet/in.h>

#include "Thread.h"
#include "Packet.h"

class Server;

class Connection: public Thread
{
    int socket;
    sockaddr_in address;
    uint32_t uid;
    Server &server;

    const int CORRUPTED_PACKETS_LIMIT;
    const std::chrono::duration<int> RECONNECTION_TIME_LIMIT;
    const timeval RECV_TIMEOUT;

    bool disconnected;
    bool identified;

    void closeSocket();
    void handlePacket(Packet packet);
public:
    Connection(int socket, sockaddr_in address, uint32_t uid, Server &server);

    Connection(const Connection &connection) = delete;
    void run() override;
    void send(Packet &packet);

    bool isDisconnected();
    bool isClosed();
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

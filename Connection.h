#pragma once

#include <netinet/in.h>

#include "Thread.h"
#include "Packet.h"

class Server;

class Connection: public Thread
{
public:
    enum class Mode {
        Idle,
        Busy
    };

private:
    int socket;
    sockaddr_in address;
    uint32_t uid;
    Server &server;
    Mode mode;
    std::chrono::steady_clock::time_point lastActive;

    std::chrono::seconds inactiveTimeout;

    const int CORRUPTED_PACKETS_LIMIT{5};
    const std::chrono::seconds INACTIVE_TIMEOUT_IDLE{60};
    const std::chrono::seconds INACTIVE_TIMEOUT_BUSY{10};
    const timeval RECV_TIMEOUT_IDLE{30, 0};
    const timeval RECV_TIMEOUT_BUSY{2, 0};
    const timeval SEND_TIMEOUT_IDLE{5, 0};
    const timeval SEND_TIMEOUT_BUSY{1, 0};

    bool disconnected;
    bool identified;

    void setMode(Mode mode);
    void closeSocket();
    void handlePacket(Packet packet);
public:
    Connection(int socket, sockaddr_in address, uint32_t uid, Server &server);

    Connection(const Connection &connection) = delete;
    void run() override;
    void send(Packet &packet);

    bool isClosed();
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

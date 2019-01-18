#pragma once

#include <netinet/in.h>
#include <unordered_map>

#include "Packet.h"
#include "../Utils/Thread.h"
#include "../Types.h"

class App;

class Connection: public Thread
{
public:
    enum class Mode
    {
        Idle,
        Busy
    };

    const std::chrono::seconds INACTIVE_TIMEOUT_IDLE{60};
    const std::chrono::seconds INACTIVE_TIMEOUT_BUSY{10};
    const timeval RECV_TIMEOUT_IDLE{20, 0};
    const timeval RECV_TIMEOUT_BUSY{2, 0};
    const timeval SEND_TIMEOUT{1, 0};
    const int CORRUPTED_PACKETS_LIMIT{5};

private:
    App &app;
    Uid uid;
    Port port;
    std::string ip;
    int socket;
    sockaddr_in address;
    Mode mode;

    std::chrono::steady_clock::time_point lastActiveAt;
    std::chrono::seconds inactiveTimeout;

public:
    Connection(App &app, Uid uid, int socket, sockaddr_in address);
    Connection(const Connection &connection) = delete;

    Uid getUid() const;
    Port getPort() const;
    std::string getIp() const;
    const sockaddr_in &getAdress() const;
    Mode getMode() const;
    void setMode(Mode mode);

    void send(const Packet &packet) const;

    void before() override;
    void run() override;
    bool stop(bool wait) override;
    void after() override;
};


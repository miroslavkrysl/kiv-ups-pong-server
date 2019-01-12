#pragma once

#include "../Utils/Thread.h"

class Server;

class ConnectionWatcher: public Thread
{
    const std::chrono::seconds CHECK_PERIOD{3};
    Server &server;

public:
    explicit ConnectionWatcher(Server &server);

    void run() override;
    bool stop(bool wait) override;
    void before() override;
    void after() override;
};



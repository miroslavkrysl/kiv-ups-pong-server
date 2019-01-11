#pragma once

#include "Thread.h"

class Server;

class ConnectionWatcher: public Thread
{
    const std::chrono::seconds CHECK_PERIOD{3};
    Server &server;

public:
    explicit ConnectionWatcher(Server &server);
    void run() override;
};



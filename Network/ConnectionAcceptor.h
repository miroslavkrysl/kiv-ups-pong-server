#pragma once

#include "../Utils/Thread.h"

class Server;

class ConnectionAcceptor : public Thread
{
    Server &server;
    int serverSocket;

public:
    explicit ConnectionAcceptor(Server &server);

    void run() override;
    bool stop(bool wait) override;
    void before() override;
    void after() override;
};



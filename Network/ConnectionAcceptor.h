#pragma once

#include "../Utils/Thread.h"

class Server;

class ConnectionAcceptor: public Thread
{
    Server &server;
    int serverSocket;

public:
    explicit ConnectionAcceptor(Server &server);

    void before() override;
    void run() override;
    bool stop(bool wait) override;
    void after() override;
};



#pragma once

#include "Thread.h"

class Server;

class ConnectionAcceptor : public Thread
{
    Server &server;
    int serverSocket;
public:
    explicit ConnectionAcceptor(Server &server);
    void run() override;
    void stop(bool wait) override;
};



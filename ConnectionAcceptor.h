#pragma once

#include "Thread.h"

class Server;

class ConnectionAcceptor : public Thread
{
    Server &server;
public:
    explicit ConnectionAcceptor(Server &server);
    void run() override;
};



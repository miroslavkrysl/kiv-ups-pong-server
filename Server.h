#pragma once

#include <netinet/in.h>
#include <stdexcept>
#include <vector>

#include "Connection.h"

class ServerException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class Server
{
    sockaddr_in serverAddress_;
    bool shouldTerminate_;
    std::vector<Connection> connections_;

public:
    explicit Server(uint16_t port, std::string ipAddress = "");
    void run();
    void terminate();
};



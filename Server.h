#pragma once

#include <stdexcept>
#include <netinet/in.h>

class ServerException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class Server
{
    sockaddr_in serverAddress_;
public:
    explicit Server(uint16_t port, std::string ipAddress = "");
    void run();
};



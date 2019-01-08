#pragma once

#include <netinet/in.h>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>

#include "Connection.h"
#include "Player.h"

class Connection;

class Server
{
    sockaddr_in serverAddress_;
    bool shouldTerminate_;

    std::map<uint32_t, Connection> connections_;
    std::unordered_map<std::string, Player> players_;

    std::mutex connectionsMutex_;
    std::mutex playersMutex_;

    uint32_t nextConnectionUid_();
    void handleConnection_(int socket, sockaddr_in address);
    void removeConnection_(uint32_t uid);

    void acceptConnections_();

public:
    explicit Server(uint16_t port, std::string ipAddress = "");
    void run();
    void terminate();
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



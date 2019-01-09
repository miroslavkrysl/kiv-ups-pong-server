#pragma once

#include <netinet/in.h>
#include <stdexcept>
#include <vector>
#include <mutex>
#include <map>
#include <unordered_map>

#include "Connection.h"
#include "Player.h"
#include "Logger.h"

class Connection;

class Server
{
    Logger &logger;

    sockaddr_in serverAddress;
    bool shouldTerminate;

    std::map<uint32_t, Connection> connections;
    std::unordered_map<std::string, Player> players;

    std::mutex connectionsMutex;
    std::mutex playersMutex;

    uint32_t nextConnectionUid_();
    void handleConnection(int socket, sockaddr_in address);
    void removeConnection(uint32_t uid);

    void acceptConnections();

public:
    explicit Server(uint16_t port, std::string ipAddress, Logger &logger_);
    void run();
    void terminate();
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



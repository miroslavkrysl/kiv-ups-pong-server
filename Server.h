#pragma once

#include <netinet/in.h>

#include <stdexcept>
#include <mutex>
#include <map>
#include <unordered_map>

#include "ConnectionAcceptor.h"
#include "ConnectionWatcher.h"
#include "Logger.h"
#include "Connection.h"
#include "Player.h"

class Server: public Thread
{
    friend class ConnectionAcceptor;
    friend class ConnectionWatcher;

    ConnectionAcceptor connectionAcceptor;
    ConnectionWatcher connectionWatcher;
    Logger &logger;
    Stats stats;

    sockaddr_in address;

    std::map<Connection::Uid, Connection> connections;
    std::unordered_map<std::string, Player> players;

    std::mutex connectionsMutex;
    std::mutex playersMutex;

    void run() override;

    void handleConnection(int socket, sockaddr_in address);

public:
    explicit Server(uint16_t port, std::string ipAddress, Logger &logger_);

    Stats &getStats();
    void stop(bool wait) override;
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



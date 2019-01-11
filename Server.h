#pragma once

#include <netinet/in.h>

#include <stdexcept>
#include <mutex>
#include <list>
#include <functional>

#include "ConnectionAcceptor.h"
#include "ConnectionWatcher.h"
#include "Logger.h"
#include "Connection.h"
#include "Player.h"

class Server: public Thread
{
    friend class ConnectionAcceptor;
    friend class ConnectionWatcher;

    Logger &logger;
    Stats stats;
    ConnectionAcceptor connectionAcceptor;
    ConnectionWatcher connectionWatcher;

    sockaddr_in address;

    std::list<Connection> connections;
    std::mutex connectionsMutex;

    void run() override;

    void handleConnection(int socket, sockaddr_in address);

public:
    explicit Server(uint16_t port, std::string ipAddress, Logger &logger_);

    Stats &getStats();
    void stop(bool wait) override;

    void addConnection(int socket, sockaddr_in address);
    void filterConnections(std::function<bool(Connection&)> filter);
    void forEachConnection(std::function<void(Connection&)> function);
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



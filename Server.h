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
    Logger &logger;
    Stats stats;
    ConnectionAcceptor connectionAcceptor;
    ConnectionWatcher connectionWatcher;

    sockaddr_in address;

    std::list<Connection> connections;
    std::mutex connectionsMutex;

public:

    explicit Server(uint16_t port, std::string ipAddress, Logger &logger_);

    Stats &getStats();
    Logger &getLogger();
    sockaddr_in &getAddress();

    Connection &addConnection(int socket, sockaddr_in address);
    size_t filterConnections(std::function<bool(Connection&)> filter);
    size_t forEachConnection(std::function<void(Connection&)> function);

    void run() override;
    bool stop(bool wait) override;
    void before() override;
    void after() override;
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



#pragma once

#include <netinet/in.h>

#include <stdexcept>
#include <mutex>
#include <list>
#include <functional>

#include "ConnectionAcceptor.h"
#include "ConnectionWatcher.h"
#include "Connection.h"
#include "../Utils/Logger.h"
#include "../Utils/Shell.h"
#include "../Game/PlayerState.h"

class Server: public Thread
{
    const std::chrono::seconds STATS_WRITE_PERIOD{3};

    Logger logger;
    Shell shell;
    Stats stats;
    ConnectionAcceptor connectionAcceptor;
    ConnectionWatcher connectionWatcher;

    sockaddr_in address;

    std::list<Connection> connections;
    std::mutex connectionsMutex;

public:

    explicit Server(uint16_t port, std::string ipAddress = "");

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



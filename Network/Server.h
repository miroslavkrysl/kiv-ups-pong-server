#pragma once

#include <netinet/in.h>

#include <stdexcept>
#include <mutex>
#include <list>
#include <functional>

#include "ConnectionAcceptor.h"
#include "Connection.h"
#include "../Utils/Logger.h"
#include "../Utils/Shell.h"

class Server: public Thread
{
    const std::chrono::seconds RUN_LOOP_PERIOD{3};

    Logger logger;
    Shell shell;
    Stats stats;
    ConnectionAcceptor connectionAcceptor;

    sockaddr_in address;
    std::string ipString;
    uint16_t port;

    std::unordered_map<Uid, std::unique_ptr<Connection>> connections;
    Uid lastUid;
    std::mutex connectionsMutex;

    std::list<std::unique_ptr<Game>> publicGames;
    std::unordered_map<std::string, std::unique_ptr<Game>> privateGames;
    std::mutex publicGamesMutex;
    std::mutex privateGamesMutex;

public:
    explicit Server(uint16_t port, std::string ipAddress = "");

    Stats &getStats();
    Logger &getLogger();
    sockaddr_in &getAddress();

    Connection &addConnection(int socket, sockaddr_in address);
    size_t clearClosedConnections();
    size_t forEachConnection(std::function<void(Connection &)> function);

    Game &joinPublic(Connection &connection);
    Game &createPrivate(Connection &connection);
    Game &joinPrivate(Connection &connection, std::string opponent);
    size_t clearEndedGames();
    size_t forEachPublicGame(std::function<void(Game &)> function);
    size_t forEachPrivateGame(std::function<void(Game &)> function);

    void before() override;
    void run() override;
    bool stop(bool wait) override;
    void after() override;

    std::string toLogString();
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

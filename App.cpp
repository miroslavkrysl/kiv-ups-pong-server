#include <utility>

#include "App.h"

App::App(Port port, std::string ip, size_t maxConnections)
    : logger{"server.log", "communication.log", "stats.log"},
      server{*this, port, std::move(ip)},
      packetHandler{*this},
      lastConnectionUid{0},
      lastGameUid{0},
      maxConnections{maxConnections}
{}

Logger &App::getLogger()
{
    return logger;
}

Server &App::getServer()
{
    return server;
}

Stats &App::getStats()
{
    return stats;
}

PacketHandler &App::getPacketHandler()
{
    return packetHandler;
}

Connection &App::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock(connectionsMutex);

    Uid uid = lastConnectionUid++;

    auto inserted = connections.insert(
        std::piecewise_construct,
        std::forward_as_tuple(uid),
        std::forward_as_tuple(*this, uid, socket, address);

    lock.unlock();

    Connection &connection = (*inserted.first->second);
    connection.start();

    return connection;
}

Connection &App::getConnection(Uid uid)
{
    // TODO
}

size_t App::clearClosedConnections()
{
    // TODO
}
size_t App::forEachConnection(std::function<void(Connection &)> function)
{
    // TODO
}
Game &App::addGame()
{
    // TODO
}
Connection &App::getGame(Uid uid)
{
    // TODO
}
size_t App::clearEndedGames()
{
    // TODO
}
size_t App::forEachGame(std::function<void(Game &)> function)
{
    // TODO
}
void App::before()
{
    // TODO
}
void App::run()
{
    // TODO
}
void App::after()
{
    // TODO
}

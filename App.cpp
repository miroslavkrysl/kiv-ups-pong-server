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

Timestamp App::getCurrentTimestamp()
{
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    );

    return ms.count();
}

void App::addNickname(Uid uid, std::string nickname)
{
    std::unique_lock<std::mutex> lock(connectionsMutex);

    connectionsNicknames.emplace(uid, nickname);
}

std::string App::getNickname(Uid uid)
{
    std::unique_lock<std::mutex> lock(connectionsNicknamesMutex);

    auto found = connectionsNicknames.find(uid);

    if (found == connectionsNicknames.end()) {
        return "";
    }

    return found->second;
}

Connection *App::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock(connectionsMutex);

    Uid uid = lastConnectionUid++;

    auto inserted = connections.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(uid),
        std::forward_as_tuple(*this, uid, socket, address));

    Connection *connection = &inserted.first->second;

    if (connections.size() > maxConnections) {
        connection->send(Packet{"server_full"});
        return nullptr;
    }

    connection->start();

    return connection;
}

Connection *App::getConnection(Uid uid)
{
    std::unique_lock<std::mutex> lock(connectionsMutex);

    auto found = connections.find(uid);

    if (found == connections.end()) {
        return nullptr;
    }

    return &found->second;
}

size_t App::clearClosedConnections()
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    size_t count{0};

    auto connectionsIt = connections.begin();
    while (connectionsIt != connections.end()) {

        if (!connectionsIt->second.isRunning()) {

            Uid uid = connectionsIt->first;

            std::unique_lock<std::mutex> lockCG{connectionsGamesMutex};
            std::unique_lock<std::mutex> lockCN{connectionsNicknamesMutex};

            // check for active game
            auto foundGame = connectionsGames.find(uid);
            if (foundGame != connectionsGames.end()) {
                foundGame->second->eventPlayerLeave(uid);
            }
            connectionsGames.erase(uid);

            // check for nickname
            connectionsNicknames.erase(uid);

            connectionsIt = connections.erase(connectionsIt);
            count++;
        } else {
            connectionsIt++;
        }
    }

    return count;
}

size_t App::forEachConnection(std::function<void(Connection &)> function)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    size_t count{0};

    for (auto &connection : connections) {
        function(connection.second);
        count++;
    }

    return count;
}

Game *App::addGame()
{
    std::unique_lock<std::mutex> lock(gamesMutex);

    Uid uid = lastGameUid++;

    auto inserted = games.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(uid),
        std::forward_as_tuple(*this, uid));

    Game *game = &inserted.first->second;

    game->start();

    return game;
}

Game *App::getGame(Uid uid)
{
    std::unique_lock<std::mutex> lock(gamesMutex);

    auto found = games.find(uid);

    if (found == games.end()) {
        return nullptr;
    }

    return &found->second;
}

size_t App::clearEndedGames()
{
    std::unique_lock<std::mutex> lock{gamesMutex};

    size_t count{0};

    auto gamesIt = games.begin();
    while (gamesIt != games.end()) {

        if (!gamesIt->second.isRunning()) {
            gamesIt = games.erase(gamesIt);
            count++;
        } else {
            gamesIt++;
        }
    }

    return count;
}

size_t App::forEachGame(std::function<void(Game &)> function)
{
    std::unique_lock<std::mutex> lock{gamesMutex};

    size_t count{0};

    for (auto &game : games) {
        function(game.second);
        count++;
    }

    return count;
}

void App::before()
{
    logger.log("starting application");
    server.start();
}

void App::run()
{
    while (!shouldStop()) {
        clearClosedConnections();
        clearEndedGames();

        auto lock = acquireLock();
        waitFor(lock, std::chrono::seconds{10});
    }
}

void App::after()
{
    logger.log("closing application");

    server.stop(true);
    forEachConnection([](Connection &connection) {
        connection.stop(true);
    });
}

#include <utility>

#include "App.h"

App::App(Port port, std::string ip, size_t maxConnections)
    : logger{"server.log", "communication.log", "stats.log"},
      server{*this, port, std::move(ip)},
      packetHandler{*this},
      lastConnectionUid{0},
      lastGameUid{0},
      maxConnections{maxConnections},
      pendingGame{nullptr}
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
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);

    connectionsNicknames.emplace(uid, nickname);
}

std::string App::getNickname(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);

    auto found = connectionsNicknames.find(uid);

    if (found == connectionsNicknames.end()) {
        return "";
    }

    return found->second;
}

void App::removeNickname(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);
    connectionsNicknames.erase(uid);
}


void App::login(Uid uid, std::string nickname)
{
    if (isLogged(uid)) {
        throw AlreadyLoggedException{std::to_string(uid) + ": " + nickname + " - aldready logged"};
    }
    addNickname(uid, nickname);
}

bool App::isLogged(Uid uid)
{
    return !getNickname(uid).empty();
}

Connection *App::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

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
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

    auto found = connections.find(uid);

    if (found == connections.end()) {
        return nullptr;
    }

    return &found->second;
}

void App::removeConnection(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

    Connection *connection = getConnection(uid);
    if (connection) {
        leaveGame(uid);
        removeNickname(uid);
        connection->stop(true);
        connections.erase(uid);
    }
}

size_t App::clearClosedConnections()
{
    std::unique_lock<std::recursive_mutex> lock{connectionsMutex};

    size_t count{0};

    auto connectionsIt = connections.begin();
    while (connectionsIt != connections.end()) {

        if (!connectionsIt->second.isRunning()) {

            Uid uid = connectionsIt->first;

            removeConnection(uid);

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
    std::unique_lock<std::recursive_mutex> lock{connectionsMutex};

    size_t count{0};

    for (auto &connection : connections) {
        function(connection.second);
        count++;
    }

    return count;
}

Game *App::addGame()
{
    std::unique_lock<std::recursive_mutex> lock(gamesMutex);

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
    std::unique_lock<std::recursive_mutex> lock(gamesMutex);

    auto found = games.find(uid);

    if (found == games.end()) {
        return nullptr;
    }

    return &found->second;
}

Game *App::joinGame(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock{gamesMutex};

    Game *game;

    if (pendingGame) {
        game = pendingGame;
        pendingGame = nullptr;
    } else {
        pendingGame = addGame();
        game = pendingGame;
    }

    game->eventPlayerJoin(uid);

    return game;
}

void App::leaveGame(Uid connectionUid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsGamesMutex);

    Game *game = getConnectionGame(connectionUid);
    if (game) {
        game->eventPlayerLeave(connectionUid);
    }
}

void App::removeGame(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(gamesMutex);

    Game *game = getGame(uid);
    if (game) {
        game->stop(true);
        games.erase(uid);
    }
}

Game *App::getConnectionGame(Uid connectionUid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsGamesMutex);

    auto found = connectionsGames.find(connectionUid);

    if (found == connectionsGames.end()) {
        return nullptr;
    }

    return getGame(found->second);
}

size_t App::clearEndedGames()
{
    std::unique_lock<std::recursive_mutex> lock{gamesMutex};

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
    std::unique_lock<std::recursive_mutex> lock{gamesMutex};

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

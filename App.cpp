#include <utility>

#include "App.h"
#include "Exceptions.h"

App::App(Port port, std::string ip, size_t maxConnections)
    : logger{"server.log", "communication.log", "stats.log"},
      shell{*this},
      server{*this, port, std::move(ip)},
      packetHandler{*this},
      lastConnectionUid{0},
      lastGameUid{0},
      maxConnections{maxConnections},
      pendingGame{nullptr}
{}

void App::addNickname(Uid uid, std::string nickname)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);

    connectionsNicknames.emplace(uid, nickname);
}

void App::removeNickname(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);

    connectionsNicknames.erase(uid);
}

Connection &App::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

    Uid uid = lastConnectionUid++;

    auto inserted = connections.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(uid),
        std::forward_as_tuple(*this, uid, socket, address));

    return inserted.first->second;
}

App::ConnectionIterator App::removeConnection(ConnectionIterator it)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

    Connection &connection = it->second;

    try {
        leaveGame(connection.getUid());
    }
    catch (NotInGameException &exception) {
        // not in a game, no need to leave
    }

    removeNickname(connection.getUid());

    connection.stop(true);

    notifyOne();
    return connections.erase(it);
}

Game &App::addGame()
{
    std::unique_lock<std::recursive_mutex> lock(gamesMutex);

    Uid uid = lastGameUid++;

    auto inserted = games.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(uid),
        std::forward_as_tuple(*this, uid));

    return inserted.first->second;;
}

App::GameIterator App::removeGame(GameIterator it)
{
    std::unique_lock<std::recursive_mutex> lockG(gamesMutex);
    std::unique_lock<std::recursive_mutex> lockPG(pendingGameMutex);

    Game &game = it->second;

    if (pendingGame && pendingGame->getUid() == game.getUid()) {
        pendingGame = nullptr;
    }

    game.stop(true);

    notifyOne();
    return games.erase(it);
}

void App::addConnectionGame(Uid connectionUid, Uid gameUid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsGamesMutex);

    auto inserted = connectionsGames.emplace(connectionUid, gameUid);
}

void App::removeConnectionGame(Uid connectionUid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsGamesMutex);

    connectionsGames.erase(connectionUid);
}

size_t App::clearClosedConnections()
{
    std::unique_lock<std::recursive_mutex> lock{connectionsMutex};

    size_t count{0};

    auto connectionsIt = connections.begin();
    while (connectionsIt != connections.end()) {

        if (!connectionsIt->second.isRunning()) {
            connectionsIt = removeConnection(connectionsIt);
            count++;
        } else {
            connectionsIt++;
        }
    }

    return count;
}

size_t App::clearEndedGames()
{
    std::unique_lock<std::recursive_mutex> lock{gamesMutex};

    size_t count{0};

    auto gamesIt = games.begin();
    while (gamesIt != games.end()) {

        if (!gamesIt->second.isRunning()) {
            gamesIt = removeGame(gamesIt);
            count++;
        } else {
            gamesIt++;
        }
    }

    return count;
}

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

Connection &App::registerConnection(int socket, sockaddr_in address)
{
    Connection &connection = addConnection(socket, address);

    if (connections.size() > maxConnections) {
        packetHandler.handleOutgoingPacket(connection.getUid(), Packet{"server_full"});

        throw ServerFullException{"server can not accept more connections"};
    }

    connection.start();

    return connection;
}

Connection &App::getConnection(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsMutex);

    auto found = connections.find(uid);

    if (found == connections.end()) {
        throw ConnectionNotExistsException{"connection " + std::to_string(uid) + " not exists"};
    }

    return found->second;
}

void App::login(Uid uid, std::string nickname)
{
    try {
        getNickname(uid);
        throw AlreadyLoggedException{std::to_string(uid) + ": " + nickname + " - already logged"};
    }
    catch (NoNicknameException &exception) {
        addNickname(uid, nickname);
    }
}

std::string App::getNickname(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsNicknamesMutex);

    auto found = connectionsNicknames.find(uid);

    if (found == connectionsNicknames.end()) {
        throw NoNicknameException{"connection " + std::to_string(uid) + " has not a nickname"};
    }

    return found->second;
}

Game &App::joinGame(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(pendingGameMutex);

    try {
        getNickname(uid);
    }
    catch (NoNicknameException &exception) {
        throw NotLoggedException("player is not logged");
    }

    Game *game;

    if (pendingGame && pendingGame->isRunning()) {
        game = pendingGame;
        pendingGame = nullptr;
    } else {
        pendingGame = &addGame();
        game = pendingGame;
        game->start();
    }

    addConnectionGame(uid, game->getUid());

    game->eventPlayerJoin(uid);

    return *game;
}

void App::leaveGame(Uid connectionUid)
{
    // throws NotInGameException if player not in a game
    Game &game = getConnectionGame(connectionUid);

    game.eventPlayerLeave(connectionUid);

    removeConnectionGame(connectionUid);
}

Game &App::getGame(Uid uid)
{
    std::unique_lock<std::recursive_mutex> lock(gamesMutex);

    auto found = games.find(uid);

    if (found == games.end()) {
        throw GameNotExistsException{"game " + std::to_string(uid) + " not exists"};
    }

    return found->second;
}

Game &App::getConnectionGame(Uid connectionUid)
{
    std::unique_lock<std::recursive_mutex> lock(connectionsGamesMutex);

    auto found = connectionsGames.find(connectionUid);

    if (found == connectionsGames.end()) {
        throw NotInGameException{"connection " + std::to_string(connectionUid) + " is not in a game"};
    }

    return getGame(found->second);
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
    shell.start();
}

void App::run()
{
    size_t count{0};

    while (!shouldStop()) {

        count = clearClosedConnections();
        if (count) {
            logger.log(std::to_string(count) + " closed connection" + (count > 1 ? "s" : "") + " cleared");
        }

        count = clearEndedGames();
        if (count) {
            logger.log(std::to_string(count) + " ended game" + (count > 1 ? "s" : "") + " cleared");
        }

        auto lock = acquireLock();
        waitFor(lock, std::chrono::seconds{10});
    }
}

void App::after()
{
    logger.log("closing application");

    // stop server from accepting new connections
    server.stop(true);

    // close active games
    forEachGame([](Game &game) {
        game.stop(true);
    });

    // close active connections
    forEachConnection([](Connection &connection) {
        connection.stop(true);
    });
}

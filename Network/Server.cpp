#include <arpa/inet.h>

#include <cstring>
#include <iostream>

#include "Server.h"
#include "../Game/Game.h"
#include "../Utils/Shell.h"

Server::Server(uint16_t port, std::string ipAddress)
    : connectionAcceptor{*this},
      port{port},
      logger{"server.log", "communication.log", "stats.log"},
      shell{*this}
{
    // set up address structure
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ipAddress.empty()) {
        address.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        address.sin_addr.s_addr = inet_addr(ipAddress.c_str());

        if (address.sin_addr.s_addr == INADDR_NONE) {
            throw ServerException("server ip address is in wrong format");
        }
    }

    char ipString[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(address.sin_addr), ipString, INET_ADDRSTRLEN);

    this->ipString = ipString;
}

Stats &Server::getStats()
{
    return stats;
}

Logger &Server::getLogger()
{
    return logger;
}

sockaddr_in &Server::getAddress()
{
    return address;
}

void Server::addNickname(std::string nickname)
{
    std::unique_lock<std::mutex> lock{nicknamesMutex};

    auto inserted = nicknames.insert(nickname);

    lock.unlock();

    if (!inserted.second) {
        throw ServerException("nickname " + nickname + " already exists");
    };
}

Connection &Server::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock(connectionsMutex);

    Uid uid = lastUid++;

    auto connectionPtr = std::make_unique<Connection>(uid, socket, address, *this);
    auto inserted = connections.insert(std::make_pair(uid, std::move(connectionPtr)));

    lock.unlock();

    Connection &connection = (*inserted.first->second);
    connection.start();

    return connection;
}

size_t Server::clearClosedConnections()
{
    size_t count{0};

    std::unique_lock<std::mutex> lock{connectionsMutex};

    auto connectionsIt = connections.begin();
    while (connectionsIt != connections.end()) {

        if (connectionsIt->second->isClosed()) {
            connectionsIt = connections.erase(connectionsIt);
            count++;
        }
        else {
            connectionsIt++;
        }
    }

    lock.unlock();

    return count;
}

size_t Server::forEachConnection(std::function<void(Connection &)> function)
{
    size_t count{0};

    std::unique_lock<std::mutex> lock{connectionsMutex};

    for (auto &uidConnectionPair : connections) {
        function(*uidConnectionPair.second);
        count++;
    }

    lock.unlock();

    return count;
}

Game &Server::joinPublic(Connection &connection)
{
    Game *game;

    std::unique_lock<std::mutex> lock{publicGamesMutex};

    if (!publicGames.empty() && !publicGames.back()->hasBothPlayers()) {
        game = publicGames.back().get();
    }
    else {
        auto gamePtr = std::make_unique<Game>(logger);
        game = gamePtr.get();
        publicGames.push_back(std::move(gamePtr));
    }

    game->start();
    game->pushEvent(std::make_unique<EventPlayerJoin>(connection));

    lock.unlock();

    return *game;
}

Game &Server::createPrivate(Connection &connection)
{
    std::unique_lock<std::mutex> lock{privateGamesMutex};

    auto gamePtr = std::make_unique<Game>(logger);
    auto inserted = privateGames.insert(std::make_pair(connection.getNickname(), std::move(gamePtr)));

    if (!inserted.second) {
        throw ServerException("players " + connection.getNickname() + " private game already exists");
    }

    Game &game = *inserted.first->second;

    game.start();
    game.pushEvent(std::make_unique<EventPlayerJoin>(connection));

    lock.unlock();

    return game;
}

Game &Server::joinPrivate(Connection &connection, std::string opponent)
{
    std::unique_lock<std::mutex> lock{privateGamesMutex};

    auto found = privateGames.find(opponent);

    if (found == privateGames.end()) {
        throw ServerException("players " + opponent + " private game does not exist");
    }

    Game &game = *found->second;
    game.pushEvent(std::make_unique<EventPlayerJoin>(connection));

    lock.unlock();

    return game;
}

size_t Server::clearEndedGames()
{
    size_t count{0};

    std::unique_lock<std::mutex> lockPublic{publicGamesMutex};

    auto publicGamesIt = publicGames.begin();
    while (publicGamesIt != publicGames.end()) {
        if (!(*publicGamesIt)->isRunning()) {
            publicGamesIt = publicGames.erase(publicGamesIt);
            count++;
        }
        else {
            publicGamesIt++;
        }
    }

    lockPublic.unlock();

    std::unique_lock<std::mutex> lockPrivate{privateGamesMutex};

    auto privateGamesIt = privateGames.begin();
    while (privateGamesIt != privateGames.end()) {

        if (privateGamesIt->second->isRunning()) {
            privateGamesIt = privateGames.erase(privateGamesIt);
            count++;
        }
        else {
            privateGamesIt++;
        }
    }

    lockPrivate.unlock();

    return count;
}

size_t Server::forEachPublicGame(std::function<void(Game &)> function)
{
    size_t count{0};

    std::unique_lock<std::mutex> lock{publicGamesMutex};

    for (auto &game : publicGames) {
        function(*game);
        count++;
    }

    lock.unlock();

    return count;
}

size_t Server::forEachPrivateGame(std::function<void(Game &)> function)
{
    size_t count{0};

    std::unique_lock<std::mutex> lock{privateGamesMutex};

    for (auto &ownerGamePair : privateGames) {
        function(*ownerGamePair.second);
        count++;
    }

    lock.unlock();

    return count;
}

void Server::before()
{
    logger.log("starting the server");

    connectionAcceptor.start();

    shell.start();
    shell.detach();
}

void Server::run()
{
    stats.setStarted(std::chrono::system_clock::now());

    // periodically do a cleanup and write stats
    while (!shouldStop()) {

        logger.writeStats(stats);

        size_t clearedConnections = clearClosedConnections();
        size_t clearedGames = clearEndedGames();

        std::this_thread::sleep_for(RUN_LOOP_PERIOD);
    }
}

void Server::after()
{
    shell.stop(false);
    connectionAcceptor.stop(true);

    logger.log("closing the connections");

    forEachConnection([](Connection &connection)
                      {
                          connection.stop(true);
                      });


    logger.log("server stopped");
}

std::string Server::toLogString()
{
    std::string string;

    string += "Listening on\n";
    string += "  ip: " + ipString + "\n";
    string += "port: " + std::to_string(port);

    return string;
}

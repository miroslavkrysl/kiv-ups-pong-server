#include <arpa/inet.h>

#include <cstring>
#include <iostream>

#include "Server.h"
#include "../Utils/Shell.h"

Server::Server(uint16_t port, std::string ipAddress)
    : connectionAcceptor{*this},
      port{port},
      ipString{ipAddress},
      logger{"server.log", "communication.log", "stats.log"},
      shell{std::cin, std::cout, *this}
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

Connection &Server::addConnection(int socket, sockaddr_in address)
{
    connectionsMutex.lock();

    Uid uid = lastUid++;

    auto connectionPtr = std::make_unique<Connection>(uid, socket, address, *this);
    auto inserted = connections.insert(std::make_pair(uid, std::move(connectionPtr)));

    connectionsMutex.unlock();

    Connection &connection = (*inserted.first->second);
    connection.start();

    return connection;
}

size_t Server::clearClosedConnections()
{
    size_t count{0};

    connectionsMutex.lock();

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

    connectionsMutex.unlock();

    return count;
}

size_t Server::forEachConnection(std::function<void(Connection &)> function)
{
    size_t count{0};

    connectionsMutex.lock();

    for (auto &uidConnectionPair : connections) {
        function(*uidConnectionPair.second);
        count++;
    }

    connectionsMutex.unlock();

    return count;
}

Game &Server::joinPublic(Connection &connection)
{
    Game *game;

    publicGamesMutex.lock();

    if (publicGames.back()->isNew()) {
        game = publicGames.back().get();
    }
    else {
        auto gamePtr = std::make_unique<Game>();
        publicGames.push_back(std::move(gamePtr));
        game = gamePtr.get();
    }

    game->addPlayer(connection);

    publicGamesMutex.unlock();

    return *game;
}

Game &Server::createPrivate(Connection &connection)
{
    privateGamesMutex.lock();

    auto gamePtr = std::make_unique<Game>();
    auto inserted = privateGames.insert(std::make_pair(connection.getNickname(), std::move(gamePtr)));

    if (!inserted.second) {
        throw ServerException("players " + connection.getNickname() + " private game already exists");
    }

    Game &game = *inserted.first->second;
    game.addPlayer(connection);

    privateGamesMutex.unlock();

    return game;
}

Game &Server::joinPrivate(Connection &connection, std::string opponent)
{
    privateGamesMutex.lock();

    auto found = privateGames.find(opponent);

    if (found == privateGames.end()) {
        throw ServerException("players " + opponent + " private game does not exist");
    }

    Game &game = *found->second;
    game.addPlayer(connection);

    privateGamesMutex.unlock();

    return game;
}

size_t Server::clearEndedGames()
{
    size_t count{0};

    publicGamesMutex.lock();

    auto publicGamesIt = publicGames.begin();
    while (publicGamesIt != publicGames.end()) {

        if ((*publicGamesIt)->isEnded()) {
            publicGamesIt = publicGames.erase(publicGamesIt);
            count++;
        }
        else {
            publicGamesIt++;
        }
    }

    publicGamesMutex.unlock();

    privateGamesMutex.lock();

    auto privateGamesIt = privateGames.begin();
    while (privateGamesIt != privateGames.end()) {

        if (privateGamesIt->second->isEnded()) {
            privateGamesIt = privateGames.erase(privateGamesIt);
            count++;
        }
        else {
            privateGamesIt++;
        }
    }

    privateGamesMutex.unlock();

    return count;
}

size_t Server::forEachPublicGame(std::function<void(Game &)> function)
{
    size_t count{0};

    publicGamesMutex.lock();

    for (auto &game : publicGames) {
        function(*game);
        count++;
    }

    publicGamesMutex.unlock();

    return count;
}

size_t Server::forEachPrivateGame(std::function<void(Game &)> function)
{
    size_t count{0};

    privateGamesMutex.lock();

    for (auto &ownerGamePair : privateGames) {
        function(*ownerGamePair.second);
        count++;
    }

    privateGamesMutex.unlock();

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

        if (stats.hasChanged()) {
            logger.writeStats(stats);
        }

        clearClosedConnections();
        clearEndedGames();

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

#include <netinet/in.h>
#include <zconf.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <thread>
#include <iostream>

#include "Server.h"

Server::Server(uint16_t port, std::string ipAddress, Logger &logger)
    : connectionAcceptor{*this},
      connectionWatcher{*this},
      logger(logger)
{
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

void Server::run()
{
    stats.setStarted(std::chrono::system_clock::now());

    connectionAcceptor.start();
    connectionWatcher.start();

    connectionAcceptor.join();
    connectionWatcher.join();

    forEachConnection([](Connection &connection)
                      {
                          connection.stop(true);
                      });
}

void Server::addConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    connections.emplace_back(socket, address, *this);

    Connection &connection = connections.back();
    connection.start();
}

void Server::filterConnections(std::function<bool(Connection &)> filter)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    auto connectionPtr = connections.begin();

    while (connectionPtr != connections.end()) {
        if (filter(*connectionPtr)) {
            connectionPtr = connections.erase(connectionPtr);
        }
        else {
            connectionPtr++;
        }
    }
}

void Server::forEachConnection(std::function<void(Connection &)> function)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    for (auto &connection : connections) {
        function(connection);
    }
}

Stats &Server::getStats()
{
    return stats;
}

sockaddr_in &Server::getAddress()
{
    return address;
}

void Server::stop(bool wait)
{
    connectionAcceptor.stop(false);
    connectionWatcher.stop(false);
    Thread::stop(wait);
}

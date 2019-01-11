#include <netinet/in.h>
#include <zconf.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <thread>

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

    connectionAcceptor.run();
    connectionWatcher.run();

    connectionAcceptor.join();
    connectionWatcher.join();
}

void Server::handleConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    Connection::Uid uid = std::chrono::system_clock::now();

    auto inserted = connections.emplace(std::piecewise_construct,
                                        std::forward_as_tuple(uid),
                                        std::forward_as_tuple(socket, address, uid, *this));

    Connection &connection = inserted.first->second;
    connection.start();
}

Stats &Server::getStats()
{
    return stats;
}

void Server::stop()
{
    connectionAcceptor.stop();
    connectionWatcher.stop();
    Thread::stop();
}

#include <netinet/in.h>
#include <zconf.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <thread>

#include "Server.h"
#include "Connection.h"
#include "Logger.h"

Server::Server(uint16_t port, std::string ipAddress, Logger &logger)
    : shouldTerminate{false},
    logger(logger)
{
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);

    if (ipAddress.empty()) {
        serverAddress.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        serverAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());

        if (serverAddress.sin_addr.s_addr == INADDR_NONE) {
            throw ServerException("server ip address is in wrong format");
        }
    }
}

void Server::run()
{
    acceptConnections();
}

void Server::acceptConnections()
{
    int returnValue;

    // create the socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // set the socket option to reuse address
    int parameter = 1;
    returnValue = setsockopt(serverSocket,
                             SOL_SOCKET,
                             SO_REUSEADDR,
                             reinterpret_cast<const void *>(&parameter),
                             sizeof(parameter));

    if (returnValue == -1) {
        throw ServerException("error while setting the socket options" + std::string{strerror(errno)});
    }

    // bind address to the socket
    returnValue = bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));

    if (returnValue != 0) {
        throw ServerException("error while binding the server address to the socket: " + std::string{strerror(errno)});
    }

    // start listening on the socket
    returnValue = listen(serverSocket, 5);

    if (returnValue != 0) {
        throw ServerException("error while starting to listen on the socket: " + std::string{strerror(errno)});
    }

    // loop for accepting new connections
    while (!shouldTerminate) {
        sockaddr_in clientAddress{};
        socklen_t addressSize = sizeof(in_addr_t);

        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &addressSize);

        if (clientSocket == -1) {
            throw ServerException("error while accepting the connection: " + std::string{strerror(errno)});
        }

        // handle the new connection
        handleConnection(clientSocket, clientAddress);
    }
}

void Server::terminate()
{
    shouldTerminate = true;
}

uint32_t Server::nextConnectionUid_()
{
    uint32_t uid{0};

    for (auto &uidConnection : connections) {
        if (uid != uidConnection.first) {
            break;
        }

        if (uid == UINT32_MAX) {
            throw ServerException("no more connection uids available");
        }

        uid++;
    }

    return uid;
}

void Server::handleConnection(int socket, sockaddr_in address)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    uint32_t uid = nextConnectionUid_();
    Server *server = this;

    auto inserted = connections.emplace(std::piecewise_construct,
                                         std::forward_as_tuple(uid),
                                         std::forward_as_tuple(socket, address, uid, server));

    Connection &connection = inserted.first->second;

    connection.start();
}

void Server::removeConnection(uint32_t uid)
{
    std::unique_lock<std::mutex> lock{connectionsMutex};

    connections.erase(connections.find(uid));
}

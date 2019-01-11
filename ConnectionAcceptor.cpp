#include <cstring>
#include <zconf.h>
#include <iostream>

#include "ConnectionAcceptor.h"
#include "Server.h"

ConnectionAcceptor::ConnectionAcceptor(Server &server)
    : server(server),
      serverSocket(-1)
{}

void ConnectionAcceptor::run()
{
    int returnValue;

    // create the socket
    serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

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
    returnValue = bind(serverSocket, reinterpret_cast<sockaddr *>(&server.getAddress()), sizeof(server.getAddress()));

    if (returnValue != 0) {
        throw ServerException("error while binding the server address to the socket: " + std::string{strerror(errno)});
    }

    // start listening on the socket
    returnValue = listen(serverSocket, 5);

    if (returnValue != 0) {
        throw ServerException("error while starting to listen on the socket: " + std::string{strerror(errno)});
    }

    // loop for accepting new connections
    while (!shouldStop()) {
        sockaddr_in clientAddress{};
        socklen_t addressSize = sizeof(in_addr_t);

        int clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &addressSize);

        if (clientSocket == -1) {

            if (errno == EBADF || errno == EINVAL) {
                // socket closed
                break;
            }

            throw ServerException("error while accepting the connection: " + std::string{strerror(errno)});
        }

        // handle the new connection
        server.addConnection(clientSocket, clientAddress);
    }

    ::close(serverSocket);
}

void ConnectionAcceptor::stop(bool wait)
{
    ::shutdown(serverSocket, SHUT_RDWR);
    Thread::stop(wait);
}

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

#include "ConnectionAcceptor.h"
#include "Server.h"

ConnectionAcceptor::ConnectionAcceptor(Server &server)
    : server(server),
      serverSocket(-1)
{}

void ConnectionAcceptor::before()
{
    int returnValue;

    // create the socket
    serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == -1) {
        std::string message = "cannot create a socket for listening: " + std::string{strerror(errno)};
        server.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // set the socket option to reuse address
    int parameter = 1;
    returnValue = setsockopt(serverSocket,
                             SOL_SOCKET,
                             SO_REUSEADDR,
                             reinterpret_cast<const void *>(&parameter),
                             sizeof(parameter));

    if (returnValue == -1) {
        std::string message = "error while setting the socket option: " + std::string{strerror(errno)};
        server.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // bind address to the socket
    returnValue = bind(serverSocket, reinterpret_cast<sockaddr *>(&server.getAddress()), sizeof(server.getAddress()));

    if (returnValue != 0) {
        std::string message = "error while binding the server address to the socket: " + std::string{strerror(errno)};
        server.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // start listening on the socket
    returnValue = listen(serverSocket, 5);

    if (returnValue != 0) {
        std::string message = "error while starting to listen on the socket: " + std::string{strerror(errno)};
        server.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }
}

void ConnectionAcceptor::run()
{
    server.getLogger().log("connection acceptor running");

    // loop for accepting the new connections
    while (!shouldStop()) {
        sockaddr_in clientAddress{};
        socklen_t addressSize = sizeof(in_addr_t);

        // accept new connection
        int clientSocket = ::accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &addressSize);

        if (clientSocket == -1) {
            // an error occurred

            if (errno == EBADF || errno == EINVAL) {
                // socket closed
                break;
            }

            server.getLogger()
                .log("error while accepting the connection: " + std::string{strerror(errno)}, Logger::Level::Error);
            return;
        }

        // handle the new connection
        Connection &connection = server.addConnection(clientSocket, clientAddress);

        server.getLogger()
            .log("accepted new connection uid=" + std::to_string(connection.getUid()) + " ip=" + connection.getIp());
    }
}

bool ConnectionAcceptor::stop(bool wait)
{
    // shutdown the server socket to break the blocking accept() call
    ::shutdown(serverSocket, SHUT_RDWR);
    return Thread::stop(wait);
}

void ConnectionAcceptor::after()
{
    ::close(serverSocket);
    serverSocket = -1;
    server.getLogger().log("connection acceptor stopped");
}

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

#include "Server.h"
#include "../App.h"
#include "../Exceptions.h"

Server::Server(App &app, Port port, std::string ip)
    : app(app),
      port(port)
{
    // set up address structure
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port = htons(port);

    if (ip.empty()) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        address.sin_addr.s_addr = ::inet_addr(ip.c_str());

        if (address.sin_addr.s_addr == INADDR_NONE) {
            throw ServerException("server ip address is in wrong format");
        }
    }

    char ipChars[INET_ADDRSTRLEN];
    ::inet_ntop(AF_INET, &(address.sin_addr), ipChars, INET_ADDRSTRLEN);

    this->ip = ipChars;
}

const sockaddr_in &
Server::getAddress() const
{
    return address;
}

std::string
Server::getIp() const
{
    return ip;
}

Port
Server::getPort() const
{
    return port;
}

void
Server::before()
{
    int returnValue;

    // create the socket
    socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (socket == -1) {
        std::string message = "cannot create a socket for listening: " + std::string{std::strerror(errno)};
        app.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // set the socket option to reuse address
    int parameter = 1;
    returnValue = ::setsockopt(socket,
                               SOL_SOCKET,
                               SO_REUSEADDR,
                               reinterpret_cast<const void *>(&parameter),
                               sizeof(parameter));

    if (returnValue == -1) {
        std::string message = "can not set the socket option to reuse address: " + std::string{std::strerror(errno)};
        app.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // bind address to the socket
    returnValue = ::bind(socket, reinterpret_cast<sockaddr *>(&address), sizeof(address));

    if (returnValue != 0) {
        std::string message = "can not bind the server address to the socket: " + std::string{strerror(errno)};
        app.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }

    // start listening on the socket
    returnValue = ::listen(socket, 5);

    if (returnValue != 0) {
        std::string message = "can not listen on the socket: " + std::string{strerror(errno)};
        app.getLogger().log(message, Logger::Level::Error);
        throw ServerException(message);
    }
}

void
Server::run()
{
    app.getLogger().log("server running");

    // loop for accepting the new connections
    while (!shouldStop()) {
        sockaddr_in clientAddress{};
        socklen_t addressSize = sizeof(in_addr_t);

        // accept new connection
        int clientSocket = ::accept(socket, reinterpret_cast<sockaddr *>(&clientAddress), &addressSize);

        if (clientSocket == -1) {
            // an error occurred

            if (errno == EBADF || errno == EINVAL) {
                // socket closed
                break;
            }

            app.getLogger()
                .log("error while accepting the connection: " + std::string{std::strerror(errno)},
                     Logger::Level::Error);
            return;
        }

        // handle the new connection
        try {
            Connection &connection = app.registerConnection(clientSocket, clientAddress);

            app.getLogger()
                .log("connection accepted: uid = " + std::to_string(connection.getUid())
                         + " ip = " + connection.getIp()
                         + " port = " + std::to_string(connection.getPort()), Logger::Level::Success);
        }
        catch (ServerFullException &exception) {
            app.getLogger()
                .log("connection refused: server full", Logger::Level::Warning);
        }
    }
}

bool
Server::stop(bool wait)
{
    // shutdown the socket to break the blocking accept() call
    ::shutdown(socket, SHUT_RDWR);
    return Thread::stop(wait);
}

void
Server::after()
{
    ::close(socket);
    socket = -1;
    app.getLogger().log("server stopped");
}

std::string
Server::toLog() const
{
    std::string string;

    string += "Listening on\n";
    string += "  ip: " + ip + "\n";
    string += "port: " + std::to_string(port);

    return string;
}

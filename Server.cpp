#include <netinet/in.h>
#include <zconf.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstring>
#include <thread>

#include "Server.h"

Server::Server(uint16_t port, std::string ipAddress)
    : shouldTerminate_{false},
{
    memset(&serverAddress_, 0, sizeof(serverAddress_));

    serverAddress_.sin_family = AF_INET;
    serverAddress_.sin_port = htons(port);

    if (ipAddress.empty()) {
        serverAddress_.sin_addr.s_addr = INADDR_ANY;
    }
    else {
        serverAddress_.sin_addr.s_addr = inet_addr(ipAddress.c_str());

        if (serverAddress_.sin_addr.s_addr == INADDR_NONE) {
            throw ServerException("server ip address is in wrong format");
        }
    }
}

void Server::run()
{
    int returnValue;

    // create the socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // set the socket option to reuse address
    int parameter = 1;
    returnValue = setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const void *>(&parameter), sizeof(parameter));

    if (returnValue == -1) {
        throw ServerException("error while setting the socket options" + std::string{strerror(errno)});
    }

    // bind address to the socket
    returnValue = bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress_), sizeof(serverAddress_));

    if (returnValue != 0){
        throw ServerException("error while binding the server address to the socket: " + std::string{strerror(errno)});
    }

    // start listening on the socket
    returnValue = listen(serverSocket, 5);

    if (returnValue != 0){
        throw ServerException("error while starting to listen on the socket: " + std::string{strerror(errno)});
    }

    // loop for accepting new connections
    while (!shouldTerminate_) {
        sockaddr_in clientAddress{};
        socklen_t addressSize = sizeof(in_addr_t);

        int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &addressSize);

        if (clientSocket == -1) {
            throw ServerException("error while accepting the connection: " + std::string{strerror(errno)});
        }

        Connection connection{clientSocket, clientAddress};

        // create an own thread for the new connection
        try {
            std::thread thread(&Connection::listen, connection);
        }
        catch (std::exception &exception) {
            throw ServerException("could not create a new thread for the incoming connection");
        }


        // TODO: handle thread watching
    }
}

void Server::terminate()
{
    shouldTerminate_ = true;
}

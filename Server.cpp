#include <netinet/in.h>
#include <cstring>
#include <zconf.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include "Server.h"

void Server::run()
{}

Server::Server(uint16_t port, std::string ipAddress)
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

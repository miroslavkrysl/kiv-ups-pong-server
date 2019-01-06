#pragma once

#include <netinet/in.h>

class Connection
{
    int socket_;
    sockaddr_in address_;

public:
    Connection(int socket, sockaddr_in address);
    void listen();
};



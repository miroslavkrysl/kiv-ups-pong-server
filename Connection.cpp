#include "Connection.h"

Connection::Connection(int socket, sockaddr_in address)
    : socket_{socket},
      address_{address}
{}

void Connection::listen()
{
}

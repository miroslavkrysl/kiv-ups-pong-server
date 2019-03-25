#include "EInvalidSocket.h"

EInvalidSocket::EInvalidSocket(int socket)
    : socket{socket}
{}

const char *EInvalidSocket::what() const noexcept
{
    std::string message{"The socket " + std::to_string(socket) + " is invalid."}
    return message.c_str();
}

#include "Socket.h"

#include <unistd.h>
#include <utility>

namespace pong::network
{

Socket::Socket()
    : fileDescriptor{NULL_FILE_DESCRIPTOR}
{}

Socket::~Socket()
{
    close();
}

void Socket::close()
{
    ::close(fileDescriptor);
}

bool Socket::isOpened()
{
    return fileDescriptor != NULL_FILE_DESCRIPTOR;
}

Socket::Socket(int fileDescriptor)
    : fileDescriptor(fileDescriptor)
{}

Socket::Socket(Socket &&other) noexcept
    : fileDescriptor(std::exchange(other.fileDescriptor, NULL_FILE_DESCRIPTOR))
{}

Socket &Socket::operator=(Socket &&other) noexcept
{
    if (&other != this)
    {
        fileDescriptor = std::exchange(other.fileDescriptor, NULL_FILE_DESCRIPTOR);
    }

    return *this;
}

}

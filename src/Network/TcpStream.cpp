#include <utility>
#include <unistd.h>
#include <chrono>

#include "TcpStream.h"
#include "Exceptions/EInvalidSocket.h"
#include "Exceptions/ETcpStreamIrecoverable.h"

TcpStream::TcpStream(TcpStream &&other) noexcept
    : socket{std::exchange(other.socket, INVALID_SOCKET)},
      ip{std::exchange(other.ip, INVALID_IP)},
      port{std::exchange(other.port, INVALID_PORT)}
{}

TcpStream &TcpStream::operator=(TcpStream &&other) noexcept
{
    socket = std::exchange(other.socket, INVALID_SOCKET);
    ip = std::exchange(other.ip, INVALID_IP);
    port = std::exchange(other.port, INVALID_PORT);
    sockaddr_in::sin_addr.s_addr
}

TcpStream::~TcpStream()
{
    close();
}

ssize_t TcpStream::send(char *buffer, size_t size)
{
    ssize_t bytesSend();
    while () {

    }
    ssize_t bytesSend = ::send(socket, buffer, size, 0);

    return bytesSend;
}

std::string TcpStream::receive()
{
    if (socket == INVALID_SOCKET) {
        throw ETcpStreamClosed{};
    }

    while (true) {
        ssize_t bytesRead = ::recv(socket, buffer, size, 0);

        if (bytesRead == -1) {
            // an error occurred

            if (errno == EINTR) {
                // the recv call was interrupted, try again
                continue;
            } else if (errno == EAGAIN) {
                // recv time out
                return 0;
            } else if (errno == EBADF || errno == EINVAL || errno == ENOTSOCK) {
                throw EInvalidSocket{socket};
            } else {
                throw ETcpStreamIrecoverable{};
            }
        }

        if (bytesRead == 0) {
            // player orderly disconnected
            throw ETcpStreamClosed{};
        }

        break;
    }

}

void TcpStream::close()
{
    if (socket == INVALID_SOCKET) {
        return;
    }

    ::close(socket);
    socket = INVALID_SOCKET;
}

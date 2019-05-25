#pragma once

#include "SocketAddress.h"

namespace pong::network
{

/**
 * Base class for BSD sockets.
 */
class Socket
{
public:
    /**
     * Value of the empty/invalid/unconnected socket file descriptor.
     */
    static const int NULL_FILE_DESCRIPTOR{-1};

    /**
     * Initialize a new empty/invalid Socket.
     */
    Socket();

    /**
     * Socket destructor.
     * Calls the close() method.
     */
    virtual ~Socket();

    /**
     * Deleted copy constructor.
     *
     * @param other The original Socket.
     */
    Socket(const Socket &other) = delete;

    /**
     * Deleted copy assignment.
     *
     * @param other The original Socket.
     * @return The copy assigned Socket.
     */
    Socket& operator=(const Socket &other) = delete;

    /**
     * Close the socket.
     */
    void close();

    /**
     * Check whether the Socket is opened.
     *
     * @return True if the Socket is opened, false otherwise.
     */
    bool isOpened();

protected:
    /**
     * Initialize a new Socket.
     *
     * @param fileDescriptor The socket file descriptor.
     */
    explicit Socket(int fileDescriptor);

    /**
     * Move construct the Socket.
     *
     * @param other The original Socket.
     */
    Socket(Socket &&other) noexcept;

    /**
     * Move assign the Socket.
     *
     * @param other The original socket.
     * @return The new Socket.
     */
    Socket& operator=(Socket&& other) noexcept;

private:
    /**
     * Socket file descriptor.
     */
    int fileDescriptor;
};

}

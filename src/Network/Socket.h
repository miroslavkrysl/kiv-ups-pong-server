#pragma once

/**
 * BSD socket wrapper.
 */
class Socket
{
    /**
     * Socket file descriptor.
     */
    int fileDescriptor;

public:
    /**
     * Value of the empty/invalid socket file descriptor.
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
     *
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
    Socket(int fileDescriptor);

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
     *
     * @return The move assigned Socket.
     */
    Socket& operator=(Socket&& other) noexcept;
};

#pragma once

#include <string>
#include <arpa/inet.h>
#include <tclDecls.h>

class TcpStream
{
public:
    /**
     * A value for invalid socket.
     */
    static const int CLOSED_SOCKET{-1};

    /**
     * A value for invalid IP.
     */
    static in_addr_t const EMPTY_IP{};

    /**
     * A value for invalid port number.
     */
    static const uint16_t EMPTY_PORT{0};

    /**
     *
     */
    inline static const std::chrono::seconds RECV_TIMEOUT{1};
    inline static const std::chrono::seconds SEND_TIMEOUT{1};

private:
    /**
     * The stream socket file descriptor.
     */
    int socket;

    /**
     * The peer ip address.
     */
    std::string ip;

    /**
     * The peer port.
     */
    uint16_t port;

public:
    friend class TcpAcceptor;

    /**
     * Deleted copy constructor.
     *
     * @param other
     */
    TcpStream(const TcpStream& other) = delete;

    /**
     * Move constructor.
     * Sets the original TcpStream socket to -1, IP to "" and port to 0.
     *
     * @param stream The original TcpStream object.
     */
    TcpStream(TcpStream&& other) noexcept;

    /**
     * The move assignment operator.
     *
     * @param other The original TcpStream object.
     *
     * @return The TcpStream object which is being assigned to.
     */
    TcpStream& operator=(TcpStream&& other) noexcept;

    /**
     * The TcpStream destructor.
     * Closes the socket.
     */
    ~TcpStream();

    /**
     * Send the buffer contents of the given size to the peer.
     *
     * @param buffer The pointer to the source buffer.
     * @param size The size of the contents to be sent.
     *
     * @return The size of the sent data.
     */
    ssize_t send(char* buffer, size_t size);

    /**
     * Receive the data into the buffer of the given size from the peer.
     *
     * @param buffer The pointer to the destination buffer.
     * @param size The size of the buffer.
     *
     * @return The size of the received data.
     */
    ssize_t receive(char* buffer, size_t size);

    /**
     * Close the TCP stream.
     */
    void close();

    /**
     * Get the peer IP address.
     * @return The peer IP address.
     */
    std::string getIP();

    /**
     * Get the peer port number.
     *
     * @return The peer port number.
     */
    uint16_t getPort();

private:

    /**
     * Initialize a connected TCP stream.
     * This constructor is private, because only the TcpAcceptor can create a connection.
     *
     * @param socket The stream socket file descriptor.
     * @param address The peer address.
     */
    TcpStream(int socket, struct sockaddr_in &address);
};

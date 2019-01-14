#pragma once

#include <netinet/in.h>
#include <unordered_map>

#include "Packet.h"
#include "../Utils/Thread.h"
#include "../Game/Game.h"

class Server;

class Connection: public Thread
{
public:
    typedef std::chrono::system_clock::time_point Uid;

    enum class Mode
    {
        Idle,
        Busy
    };

private:
    int socket;
    const sockaddr_in address;
    Server &server;
    Mode mode;
    std::string ipString;

    std::chrono::steady_clock::time_point lastRecvAt;
    std::chrono::steady_clock::time_point lastSendAt;

    std::chrono::seconds inactiveTimeout;

    const int CORRUPTED_PACKETS_LIMIT{5};
    const std::chrono::seconds INACTIVE_TIMEOUT_IDLE{60};
    const std::chrono::seconds INACTIVE_TIMEOUT_BUSY{10};
    const timeval RECV_TIMEOUT_IDLE{30, 0};
    const timeval RECV_TIMEOUT_BUSY{2, 0};
    const timeval SEND_TIMEOUT_IDLE{5, 0};
    const timeval SEND_TIMEOUT_BUSY{1, 0};

    std::string nickname;
    Game *game;

    typedef void (Connection::*Handler)(Packet);

    std::unordered_map<std::string, Handler> handlers{
        {"login", &Connection::handleLogin},
        {"join_random_game", &Connection::handleJoinRandomGame},
        {"join_private_game", &Connection::handleJoinPrivateGame},
        {"update_state", &Connection::handleUpdateState},
        {"ball_hit", &Connection::handleBallHit},
        {"leave_game", &Connection::handleLeaveGame},
        {"poke", &Connection::handlePoke},
    };

    void setMode(Mode mode);
    void handlePacket(Packet packet);

    void handleLogin(Packet packet);
    void handleJoinRandomGame(Packet packet);
    void handleJoinPrivateGame(Packet packet);
    void handleUpdateState(Packet packet);
    void handleBallHit(Packet packet);
    void handleLeaveGame(Packet packet);
    void handlePoke(Packet packet);

public:
    Connection(int socket, sockaddr_in address, Server &server);
    Connection(const Connection &connection) = delete;

    void send(Packet &packet);
    bool isClosed();
    bool isIdentified();
    std::string getId();

    void run() override;
    bool stop(bool wait) override;
    void before() override;
    void after() override;
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class NonContextualPacketException: public PacketException
{
    using PacketException::PacketException;
};

class MalformedPacketException: public PacketException
{
    using PacketException::PacketException;
};

class InvalidPacketException: public PacketException
{
    using PacketException::PacketException;
};

#pragma once

#include <netinet/in.h>
#include <unordered_map>

#include "Packet.h"
#include "../Utils/Thread.h"

class Server;
class Game;

typedef int32_t Uid;

enum class ConnectionMode
{
    Idle,
    Busy
};

class Connection: public Thread
{
    const std::chrono::seconds INACTIVE_TIMEOUT_IDLE{60};
    const std::chrono::seconds INACTIVE_TIMEOUT_BUSY{10};
    const timeval RECV_TIMEOUT_IDLE{20, 0};
    const timeval RECV_TIMEOUT_BUSY{2, 0};
    const timeval SEND_TIMEOUT{1, 0};
    const int CORRUPTED_PACKETS_LIMIT{5};

    typedef void (Connection::*PacketHandler)(Packet);

    const std::unordered_map<std::string, PacketHandler> PACKET_HANDLERS{
        // connection test
        {"poke", &Connection::handlePoke},
        {"poke_back", &Connection::handlePokeBack},
        // connection management
        {"login", &Connection::handleLogin},
        {"join_random_game", &Connection::handleJoinRandomGame},
        {"join_private_game", &Connection::handleJoinPrivateGame},
        {"create_private_game", &Connection::handleCreatePrivateGame},
        // game
        {"get_time", &Connection::handleGetTime},
        {"ready", &Connection::handleReady},
        {"restart", &Connection::handleRestart},
        {"update_state", &Connection::handleUpdateState},
        {"leave_game", &Connection::handleLeaveGame},
    };

    Server &server;
    Game *game;

    Uid uid;
    std::string nickname;
    std::string ip;
    int socket;
    const sockaddr_in address;
    ConnectionMode mode;

    std::chrono::steady_clock::time_point lastRecvAt;
    std::chrono::seconds inactiveTimeout;

    void setMode(ConnectionMode mode);
    void handlePacket(Packet packet);

    void handleLogin(Packet packet);
    void handlePoke(Packet packet);
    void handlePokeBack(Packet packet);
    void handleJoinRandomGame(Packet packet);
    void handleJoinPrivateGame(Packet packet);
    void handleCreatePrivateGame(Packet packet);
    void handleLeaveGame(Packet packet);
    void handleGetTime(Packet packet);
    void handleReady(Packet packet);
    void handleRestart(Packet packet);
    void handleUpdateState(Packet packet);

public:
    Connection(Uid uid, int socket, sockaddr_in address, Server &server);
    Connection(const Connection &connection) = delete;

    Uid getUid();
    std::string getUidStr();
    std::string getNickname();
    std::string getIp();
    bool isIdentified();
    bool isClosed();

    void send(Packet &packet);

    void before() override;
    void run() override;
    bool stop(bool wait) override;
    void after() override;
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class UnknownPacketException: public PacketException
{
    using PacketException::PacketException;
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

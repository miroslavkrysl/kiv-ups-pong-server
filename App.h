#pragma once

#include <functional>

#include "Util/Logger.h"
#include "Types.h"
#include "Networ/Server.h"
#include "Networ/Connection.h"
#include "Networ/PacketHandler.h"
#include "Gam/Game.h"

class App: public Thread
{
public:
    static const Port DEFAULT_PORT{8191};
    static const size_t DEFAUL_MAX_CONNECTIONS{120};

private:
    Logger logger;
    Server server;
    Stats stats;
    PacketHandler packetHandler;
    std::unordered_map<Uid, Connection> connections;
    std::unordered_map<Uid, Game> games;
    std::unordered_map<Uid, Game *> connectionsGames;
    std::unordered_map<Uid, std::string> connectionsNicknames;

    size_t maxConnections;
    Uid lastConnectionUid;
    Uid lastGameUid;

    std::mutex connectionsMutex;
    std::mutex gamesMutex;
    std::mutex connectionsGamesMutex;
    std::mutex connectionsNicknamesMutex;

public:
    explicit App(Port port = DEFAULT_PORT, std::string ip = "", size_t maxConnections = DEFAUL_MAX_CONNECTIONS);
    App(App &app) = delete;

    Logger &getLogger();
    Server &getServer();
    Stats &getStats();
    PacketHandler &getPacketHandler();
    Timestamp getCurrentTimestamp();

    void addNickname(Uid uid, std::string nickname);
    std::string getNickname(Uid uid);

    Connection *addConnection(int socket, sockaddr_in address);
    Connection *getConnection(Uid uid);
    size_t clearClosedConnections();
    size_t forEachConnection(std::function<void(Connection &)> function);

    Game *addGame();
    Game *getGame(Uid uid);
    size_t clearEndedGames();
    size_t forEachGame(std::function<void(Game & )> function);

    void before() override;
    void run() override;
    void after() override;
};



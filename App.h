#pragma once

#include <functional>
#include <unordered_map>

#include "Types.h"
#include "Util/Logger.h"
#include "Network/Server.h"
#include "Network/Connection.h"
#include "Network/PacketHandler.h"
#include "Game/Game.h"

class App: public Thread
{
public:
    static const Port DEFAULT_PORT{8191};
    static const size_t DEFAUL_MAX_CONNECTIONS{120};

    typedef std::unordered_map<Uid, Connection>::iterator ConnectionIterator;
    typedef std::unordered_map<Uid, Game>::iterator GameIterator;

private:
    Logger logger;
    Server server;
    Stats stats;
    PacketHandler packetHandler;
    std::unordered_map<Uid, Connection> connections;
    std::unordered_map<Uid, Game> games;
    std::unordered_map<Uid, Uid> connectionsGames;
    std::unordered_map<Uid, std::string> connectionsNicknames;

    size_t maxConnections;

    Uid lastConnectionUid;
    Uid lastGameUid;
    Game *pendingGame;

    std::recursive_mutex connectionsMutex;
    std::recursive_mutex gamesMutex;
    std::recursive_mutex pendingGameMutex;
    std::recursive_mutex connectionsGamesMutex;
    std::recursive_mutex connectionsNicknamesMutex;

    void addNickname(Uid uid, std::string nickname);
    void removeNickname(Uid uid);

    Connection &addConnection(int socket, sockaddr_in address);
    ConnectionIterator removeConnection(ConnectionIterator it);

    Game &addGame();
    GameIterator removeGame(GameIterator it);

    void addConnectionGame(Uid connectionUid, Uid gameUid);
    void removeConnectionGame(Uid uid);

    size_t clearClosedConnections();
    size_t clearEndedGames();

public:
    explicit App(Port port = DEFAULT_PORT, std::string ip = "", size_t maxConnections = DEFAUL_MAX_CONNECTIONS);
    App(App &app) = delete;

    Logger &getLogger();
    Server &getServer();
    Stats &getStats();
    PacketHandler &getPacketHandler();
    Timestamp getCurrentTimestamp();

    Connection &registerConnection(int socket, sockaddr_in address);
    Connection &getConnection(Uid uid);

    void login(Uid uid, std::string nickname);
    std::string getNickname(Uid uid);

    Game &joinGame(Uid connectionUid);
    void leaveGame(Uid connectionUid);
    Game &getGame(Uid uid);
    Game &getConnectionGame(Uid connectionUid);

    size_t forEachConnection(std::function<void(Connection &)> function);
    size_t forEachGame(std::function<void(Game &)> function);

    void before() override;
    void run() override;
    void after() override;
};



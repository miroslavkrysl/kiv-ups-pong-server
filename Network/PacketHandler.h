#pragma once

#include <unordered_map>

#include "Packet.h"
#include "../Types.h"

class App;

class PacketHandler
{
    App &app;

    typedef void (PacketHandler::*Handler)(Uid, Packet);

    const std::unordered_map<std::string, Handler> PACKET_HANDLERS{
        // connection test
        {"poke", &PacketHandler::handlePoke},
        {"poke_back", &PacketHandler::handlePokeBack},
        // connection management
        {"login", &PacketHandler::handleLogin},
        {"join", &PacketHandler::handleJoin},
        // game
        {"time", &PacketHandler::handleTime},
        {"ready", &PacketHandler::handleReady},
        {"restart", &PacketHandler::handleRestart},
        {"state", &PacketHandler::handleState},
        {"leave", &PacketHandler::handleLeave},
    };

    void handleLogin(Uid uid, Packet packet);
    void handlePoke(Uid uid, Packet packet);
    void handlePokeBack(Uid uid, Packet packet);
    void handleJoin(Uid uid, Packet packet);
    void handleLeave(Uid uid, Packet packet);
    void handleTime(Uid uid, Packet packet);
    void handleReady(Uid uid, Packet packet);
    void handleRestart(Uid uid, Packet packet);
    void handleState(Uid uid, Packet packet);

    void validateItemsCount(Packet packet, size_t count);

public:
    explicit PacketHandler(App &app);
    void handleIncomingPacket(Uid uid, const Packet &packet);
    void handleOutgoingPacket(Uid uid, const Packet &packet);
};



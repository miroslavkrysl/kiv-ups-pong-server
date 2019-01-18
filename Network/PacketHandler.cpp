#include <regex>

#include "PacketHandler.h"
#include "../App.h"
#include "../Exceptions.h"

PacketHandler::PacketHandler(App &app)
    : app(app)
{}

void PacketHandler::handleIncomingPacket(Uid uid, const Packet &packet)
{
    auto typeHandler = PACKET_HANDLERS.find(packet.getType());

    if (typeHandler == PACKET_HANDLERS.end()) {
        throw UnknownPacketException{"unknown packet type"};
    }

    Handler handler = typeHandler->second;

    try {
        (this->*handler)(uid, packet);
    }
    catch (AlreadyLoggedException &exception) {
        handleOutgoingPacket(uid, Packet{"already_logged"});
        throw NonContextualPacketException{"player already logged"};
    }
    catch (NotLoggedException &exception) {
        handleOutgoingPacket(uid, Packet{"not_logged"});
        throw NonContextualPacketException{"player not logged"};
    }
    catch (GameTypeException &exception) {
        throw MalformedPacketException{exception.what()};
    }
    catch (GameException &exception) {
        app.getStats().addPacketsDropped(1);
    }
}

void PacketHandler::handleOutgoingPacket(Uid uid, const Packet &packet)
{
    try {
        Connection &connection = app.getConnection(uid);
        connection.send(packet);
    }
    catch (ConnectionNotExistsException &exception){
        // can not send packet
    }
}

void PacketHandler::handleLogin(Uid uid, Packet packet)
{
    auto items = packet.getItems();

    if (items.size() != 1) {
        throw MalformedPacketException{"login packet must have only 1 argument: nickname"};
    }

    std::regex nicknameRegex("[a-zA-Z0-9]{3,16}");
    std::string nickname = *items.begin();

    Packet response{};

    if (std::regex_match(nickname, nicknameRegex)) {
        app.login(uid, nickname);
        handleOutgoingPacket(uid, Packet{"logged"});
    }
    else {
        handleOutgoingPacket(uid, Packet{"login_failed", {"format"}});
    }
}

void PacketHandler::handlePoke(Uid uid, Packet packet)
{
    handleOutgoingPacket(uid, Packet{"poke_back"});
}

void PacketHandler::handlePokeBack(Uid uid, Packet packet)
{}

void PacketHandler::handleJoin(Uid uid, Packet packet)
{
    app.joinGame(uid);
}

void PacketHandler::handleLeave(Uid uid, Packet packet)
{
    app.leaveGame(uid);
}

void PacketHandler::handleTime(Uid uid, Packet packet)
{
    // TODO
}

void PacketHandler::handleReady(Uid uid, Packet packet)
{
    // TODO
}

void PacketHandler::handleRestart(Uid uid, Packet packet)
{
    // TODO
}

void PacketHandler::handleState(Uid uid, Packet packet)
{
    // TODO
}

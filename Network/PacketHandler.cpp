#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <regex>

#include "PacketHandler.h"
#include "../App.h"
#include "../Exceptions.h"

PacketHandler::PacketHandler(App &app)
    : app(app)
{}

void PacketHandler::validateItemsCount(Packet packet, size_t count)
{
    if (packet.getItems().size() != count) {
        throw MalformedPacketException{"packet must have " + std::to_string(count) + " parameters"};
    }
}

void PacketHandler::handleIncomingPacket(Uid uid, const Packet &packet)
{
    auto typeHandler = PACKET_HANDLERS.find(packet.getType());

    if (typeHandler == PACKET_HANDLERS.end()) {
        handleOutgoingPacket(uid, Packet{"unknown_packet"});
        throw UnknownPacketException{"unknown packet type"};
    }

    Handler handler = typeHandler->second;

    try {
        (this->*handler)(uid, packet);
    }
    catch (AlreadyLoggedException &exception) {
        handleOutgoingPacket(uid, Packet{"already_logged"});
        throw NonContextualPacketException{"player is already logged"};
    }
    catch (NotLoggedException &exception) {
        handleOutgoingPacket(uid, Packet{"not_logged"});
        throw NonContextualPacketException{"player is not logged"};
    }
    catch (AlreadyInGameException &exception) {
        handleOutgoingPacket(uid, Packet{"already_in_game"});
        throw NonContextualPacketException{"player is already in a game"};
    }
    catch (NotInGameException &exception) {
        handleOutgoingPacket(uid, Packet{"not_in_game"});
        throw NonContextualPacketException{"player is not in a game"};
    }
    catch (GameTypeException &exception) {
        throw MalformedPacketException{exception.what()};
    }
    catch (ImpossiblePlayerStateException &exception) {
        handleOutgoingPacket(uid, Packet{"impossible_state"});
        throw MalformedPacketException{exception.what()};
    }
    catch (GamePhaseException &exception) {
        // game is not in the phase to receive this type of packet
        throw MalformedPacketException{exception.what()};
    }
    catch (GameException &exception) {
        app.getLogger().log("game exception problem: " + std::string(exception.what()), Logger::Level::Error);
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

    validateItemsCount(packet, 1);

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
    validateItemsCount(std::move(packet), 0);
    handleOutgoingPacket(uid, Packet{"poke_back"});
}

void PacketHandler::handlePokeBack(Uid uid, Packet packet)
{
    validateItemsCount(std::move(packet), 0);
}

void PacketHandler::handleJoin(Uid uid, Packet packet)
{
    validateItemsCount(std::move(packet), 0);
    app.joinGame(uid);
}

void PacketHandler::handleLeave(Uid uid, Packet packet)
{
    validateItemsCount(std::move(packet), 0);
    app.leaveGame(uid);
}

void PacketHandler::handleTime(Uid uid, Packet packet)
{
    validateItemsCount(packet, 1);

    packet.addItem(timestampToStr(app.getCurrentTimestamp()));
    handleOutgoingPacket(uid, packet);
}

void PacketHandler::handleReady(Uid uid, Packet packet)
{
    validateItemsCount(std::move(packet), 0);
    app.getConnectionGame(uid).eventPlayerReady(uid);
}

void PacketHandler::handleRestart(Uid uid, Packet packet)
{
    validateItemsCount(std::move(packet), 0);
    app.getConnectionGame(uid).eventPlayerRestart(uid);
}

void PacketHandler::handleState(Uid uid, Packet packet)
{
    validateItemsCount(packet, PlayerState::ITEMS_COUNT);
    PlayerState state{packet.getItems()};
    app.getConnectionGame(uid).eventPlayerUpdate(uid, state);
}

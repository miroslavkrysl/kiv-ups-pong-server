#include "Packet.h"

std::map<Packet::PacketType, std::string> Packet::getTypeToStringMap()
{
    if (typeStrings.empty()) {
        typeStrings = std::map<PacketType, std::string>{
            {PacketType::Empty, "empty"},
            {PacketType::LoginOk, "login_ok"},
            {PacketType::LoginFailed, "login_failed"},
            {PacketType::GameJoined, "game_joined"},
            {PacketType::GameNotJoined, "game_not_joined"},
            {PacketType::GamePaused, "game_paused"},
            {PacketType::OpponentLeaved, "opponent_leaved"},
            {PacketType::OpponentState, "opponent_state"},
            {PacketType::BallState, "ball_state"},
            {PacketType::NewRound, "new_round"},
            {PacketType::GameEnded, "game_ended"},
            {PacketType::Time, "time"},
        };
    }

    return typeStrings;
}

std::string Packet::typeToString(Packet::PacketType type)
{
    auto typeToStringMap = getTypeToStringMap();
    auto found = typeToStringMap.find(type);

    if (found == typeToStringMap.end()) {
        throw PacketException("no string representation for packet type " + std::to_string(static_cast<int>(type)));
    }

    return found->second;
}

Packet::PacketType Packet::stringToType(std::string type)
{
    auto typeToStringMap = getTypeToStringMap();

    for (auto &typeString : typeToStringMap) {
        if (typeString.second == type) {
            return typeString.first;
        }
    }

    throw PacketException("no packet type for string representation " + type);
}

Packet::Packet(PacketType type)
    : type{type}
{}

Packet::Packet(std::string contents)
    : type{PacketType::Empty}
{
    // TODO: make packet from serialized string
}

void Packet::addItem(std::string &item)
{
    items.push_back(item);
}

std::vector<std::string> &Packet::getItems()
{
    return items;
}

std::string Packet::serialize()
{
    std::string serialized;

    serialized += typeToString(type);

    for (auto &item : items) {
        serialized += DELIMITER;
        serialized += item;
    }

    serialized += TERMINATOR;

    if (serialized.size() >= MAX_SIZE) {
        throw PacketException("packet length exceeded");
    }

    return serialized;
}

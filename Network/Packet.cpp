#include <vector>
#include <sstream>

#include "Packet.h"
#include "../Exceptions.h"

constexpr std::array<char, 3> Packet::TERMINATOR = {"\r\n"};

Packet Packet::parse(const std::string &contents)
{
    // tokenize by delimiter
    std::vector<std::string> tokens;
    std::stringstream ss{contents};

    while (!ss.eof()) {
        std::string token;
        std::getline(ss, token, DELIMITER);
        tokens.push_back(token);
    }

    // first token is token type name
    std::string type = tokens.front();
    tokens.erase(tokens.begin());

    return Packet{type, tokens};
}

Packet::Packet(std::string type)
    : type{std::move(type)}
{}

Packet::Packet(std::string type, std::vector<std::string> items)
    : type{std::move(type)},
      items{std::move(items)}
{}

std::string Packet::getType() const
{
    return type;
}

std::vector<std::string> Packet::getItems() const
{
    return items;
}

std::string Packet::serialize() const
{
    std::string serialized;

    serialized += type;

    for (auto &item : items) {
        serialized += DELIMITER;
        serialized += item;
    }

    serialized += TERMINATOR.data();

    if (serialized.size() >= MAX_SIZE) {
        throw PacketException("packet length exceeded");
    }

    return serialized;
}

void Packet::clear()
{
    type.clear();
    items.clear();
}

void Packet::addItem(std::string item)
{
    items.push_back(item);
}

void Packet::addItems(std::vector<std::string> items)
{
    for (auto &item : items) {
        this->items.push_back(item);
    }
}

void Packet::setType(std::string type)
{
    this->type = std::move(type);
}
std::string Packet::toLog() const
{
    std::string serialized;

    serialized += type;

    for (auto &item : items) {
        serialized += DELIMITER;
        serialized += item;
    }

    return serialized;
}

#include <utility>

#include <utility>

#include <list>
#include <sstream>
#include <iostream>

#include "Packet.h"

Packet::Packet(std::string type)
    : type{std::move(type)}
{}

Packet::Packet(std::string type, std::list<std::string> items)
    : type{std::move(type)},
      items{std::move(items)}
{}

void Packet::parse(std::string contents)
{
    // remove termination character
    if (contents.back() == TERMINATOR) {
        contents.pop_back();
    }

    // tokenize by delimiter
    std::list<std::string> tokens;
    std::stringstream ss{contents};

    while (!ss.eof()) {
        std::string token;
        std::getline(ss, token, DELIMITER);
        tokens.push_back(token);
    }

    // first token is token type name
    type = tokens.front();
    tokens.pop_front();

    // erase existing items and fill with parsed
    clear();

    for (auto &token : tokens) {
        addItem(token);
    }
}

std::string Packet::serialize()
{
    std::string serialized;

    serialized += type;

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

void Packet::clear()
{
    type.clear();
    items.clear();
}

void Packet::addItem(std::string item)
{
    items.push_back(item);
}

std::list<std::string> &Packet::getItems()
{
    return items;
}

std::string Packet::getType()
{
    return type;
}

void Packet::setType(std::string type)
{
    this->type = std::move(type);
}

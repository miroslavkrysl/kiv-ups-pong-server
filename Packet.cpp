#include <list>
#include <sstream>
#include <iostream>

#include "Packet.h"

Packet::Packet(std::string type)
    : type{std::move(type)}
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
    items.clear();

    for (auto &token : tokens) {
        addItem(token);
    }
}

void Packet::addItem(std::string &item)
{
    items.push_back(item);
}

void Packet::addItem(int32_t &item)
{
    std::string string = std::to_string(item);
    addItem(string);
}

void Packet::addItem(uint32_t &item)
{
    std::string string = std::to_string(item);
    addItem(string);
}

std::vector<std::string> &Packet::getItems()
{
    return items;
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

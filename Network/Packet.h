#pragma once

#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include "../Utils/Serializable.h"

class Packet: public Serializable
{
public:
    static const char DELIMITER{';'};
    static const char TERMINATOR{'\n'};
    static const size_t MAX_SIZE{1024};

private:
    std::string type;
    std::list<std::string> items;

public:
    explicit Packet(std::string type = "");
    explicit Packet(std::string type, std::list<std::string> items);

    void parse(std::string contents) override;
    std::string serialize() override;
    void clear() override;

    void addItem(std::string &item);

    std::list<std::string> &getItems();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

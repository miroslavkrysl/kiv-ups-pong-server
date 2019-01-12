#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "Serializable.h"

class Packet: public Serializable
{
public:
    static const char DELIMITER{';'};
    static const char TERMINATOR{'\n'};
    static const size_t MAX_SIZE{1024};

private:
    std::string type;
    std::vector<std::string> items;

public:
    explicit Packet(std::string type = "");

    void parse(std::string contents) override;
    std::string serialize() override;
    void clear() override;

    void addItem(std::string &item);
    void addItem(Serializable &item);

    std::vector<std::string> &getItems();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

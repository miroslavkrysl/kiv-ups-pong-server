#pragma once

#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include <array>

class Packet
{
public:
    static const char DELIMITER{';'};
    static const std::array<char, 3> TERMINATOR;
    static const size_t MAX_SIZE{1024};

private:
    std::string type;
    std::list<std::string> items;

public:
    explicit Packet(std::string type = "");
    explicit Packet(std::string type, std::list<std::string> items);

    void parse(std::string contents);
    std::string serialize();
    std::string toLogString();
    void clear();

    void addItem(std::string item);

    void setType(std::string type);
    std::string getType();
    std::list<std::string> &getItems();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

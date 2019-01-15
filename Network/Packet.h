#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <array>

class Packet
{
public:
    static const char DELIMITER{';'};
    static const std::array<char, 3> TERMINATOR;
    static const size_t MAX_SIZE{1024};

    static Packet parse(const std::string &contents);

private:
    std::string type;
    std::vector<std::string> items;

public:
    explicit Packet(std::string type = "");
    explicit Packet(std::string type, std::vector<std::string> items);

    std::string getType();
    std::vector<std::string> &getItems();
    std::string serialize();
    void clear();
    void addItem(std::string item);
    void setType(std::string type);

    std::string toLogString();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

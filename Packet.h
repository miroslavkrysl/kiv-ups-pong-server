#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

class Packet
{
public:
    static const char DELIMITER{';'};
    static const char TERMINATOR{'\n'};
    static const size_t MAX_SIZE{1024};

private:
    std::string type;
    std::vector<std::string> items;

public:
    explicit Packet(std::string type = "empty");

    void parse(std::string contents);
    std::string serialize();

    void addItem(std::string &item);
    void addItem(int32_t &item);
    void addItem(uint32_t &item);

    std::vector<std::string> &getItems();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

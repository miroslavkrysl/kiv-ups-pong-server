#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

class Packet
{
public:
    enum class PacketType
    {
        Empty,
        Poke,
        PokeBack,
        // server packets
        LoginOk,
        LoginFailed,
        GameJoined,
        GameNotJoined,
        GamePaused,
        OpponentLeft,
        OpponentState,
        BallState,
        NewRound,
        GameEnded,
        Time,
        // client packets
        // TODO: add client packets
    };
private:
    static std::map<PacketType, std::string> typeStrings;

public:
    static const char DELIMITER{';'};
    static const char TERMINATOR{'\n'};
    static const size_t MAX_SIZE{1024};

    static std::map<PacketType, std::string> getTypeToStringMap();
    static PacketType stringToType(std::string);
    static std::string typeToString(PacketType type);

private:
    PacketType type;
    std::vector<std::string> items;

public:
    explicit Packet(PacketType type);
    explicit Packet(std::string contents);

    void addItem(std::string &item);
    std::vector<std::string> &getItems();
    std::string serialize();
};

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

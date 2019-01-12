#include "PlayerDirection.h"

PlayerDirection::PlayerDirection(PlayerDirection::Dir direction)
    : direction(direction)
{}

void PlayerDirection::parse(std::string data)
{
    if (data == "up") {
        direction = Dir::Up;
    }
    else if (data == "down") {
        direction = Dir::Down;
    }
    else if (data == "stop") {
        direction = Dir::Stop;
    }
    else {
        throw SerializableException("player direction is in invalid format");
    }
}

std::string PlayerDirection::serialize()
{
    if (direction == Dir::Up) {
        return "up";
    }
    else if (direction == Dir::Down) {
        return "down";
    }

    return "stop";
}

void PlayerDirection::clear()
{
    direction = Dir::Stop;
}


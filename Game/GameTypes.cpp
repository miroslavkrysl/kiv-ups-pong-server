#include "GameTypes.h"
#include "../Exceptions.h"

bool isValidPlayerPosition(int position)
{
    return position <= PLAYER_POSITION_MAX && position >= PLAYER_POSITION_MIN;
}

Timestamp strToTimestamp(std::string &str)
{
    long long timestamp;

    try {
        timestamp = std::stoll(str);
    }
    catch (...) {
        throw GameTypeException("timestamp is in invalid format");
    }

    return static_cast<Timestamp>(timestamp);
}

Position strToPlayerPosition(std::string &str)
{
    int position;

    try {
        position = std::stoi(str);

        if (!isValidPlayerPosition(position)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("player position is in invalid format");
    }

    return static_cast<Position>(position);
}

PlayerDirection strToPlayerDirection(std::string &str)
{
    if (str == "up") {
        return PlayerDirection::Up;
    }
    else if (str == "down") {
        return PlayerDirection::Down;
    }
    else if (str == "stop") {
        return PlayerDirection::Stop;
    }
    else {
        throw GameTypeException("direction is in invalid format");
    }
}

std::string timestampToStr(Timestamp timestamp)
{
    return std::to_string(timestamp);
}

std::string speedToStr(Speed speed)
{
    return std::to_string(speed);
}

std::string scoreToStr(Score score)
{
    return std::to_string(score);
}

std::string playerPositionToStr(Position position)
{
    return std::to_string(position);
}

std::string directionToStr(PlayerDirection direction)
{
    switch (direction) {
    case PlayerDirection::Up:return "up";
    case PlayerDirection::Down:return "down";
    case PlayerDirection::Stop:return "stop";
    }
}

std::string ballPositionToStr(Position position)
{
    return std::to_string(position);
}

std::string angleToStr(Angle angle)
{
    return std::to_string(angle);
}

std::string sideToStr(Side side)
{
    switch (side) {
    case Side::Left:return "left";
    case Side::Right:return "right";
    }
}

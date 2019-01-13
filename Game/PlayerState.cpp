#include <sstream>

#include "PlayerState.h"
#include "../Utils/Exceptions.h"

PlayerState::PlayerState(int16_t position, Direction direction)
    : position{position},
      direction{direction}
{
    if (!isValidPosition(position)) {
        throw EntityException("player position is in invalid format");
    }
}

PlayerState::PlayerState(std::list<std::string> items)
{
    if (items.size() != 2) {
        throw EntityException("too few items to create a PlayerState");
    }

    auto itemPtr = items.begin();
    int position;

    try {
        position = std::stoi(*itemPtr);

        if (!isValidPosition(position)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw EntityException("player position is in invalid format");
    }

    itemPtr++;
    Direction direction;

    if (*itemPtr == "up") {
        direction = Direction::Up;
    }
    else if (*itemPtr == "down") {
        direction = Direction::Down;
    }
    else if (*itemPtr == "stop") {
        direction = Direction::Stop;
    }
    else {
        throw EntityException("player direction is in invalid format");
    }

    this->position = static_cast<int16_t>(position);
    this->direction = direction;
}

void PlayerState::itemize(std::list<std::string> &destination)
{
    destination.clear();
    destination.push_back(std::to_string(position));

    std::string dir;

    switch (direction) {
    case Direction::Up:
        dir = "up";
        break;
    case Direction::Down:
        dir = "down";
        break;
    case Direction::Stop:
        dir = "stop";
        break;
    }

    destination.push_back(dir);
}

int16_t PlayerState::getPosition()
{
    return position;
}

PlayerState::Direction PlayerState::getDirection()
{
    return direction;
}

bool PlayerState::isValidPosition(int position)
{
    return position <= POSITION_MAX && position >= POSITION_MIN;
}

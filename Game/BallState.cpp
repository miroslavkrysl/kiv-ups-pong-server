#include "BallState.h"
#include "Game.h"
#include "../Utils/Exceptions.h"

BallState::BallState()
    : position{0},
      direction{0},
      speed{Game::BALL_SPEED_MIN}
{}

BallState::BallState(int16_t position, int8_t direction, int16_t speed)
    : position{position},
      direction{direction},
      speed{speed}
{
    if (!isValidPosition(position)) {
        throw EntityException("ball position is in invalid format");
    }
    if (!isValidDirection(direction)) {
        throw EntityException("ball direction is in invalid format");
    }
    if (!isValidSpeed(speed)) {
        throw EntityException("ball speed is in invalid format");
    }
}

BallState::BallState(std::list<std::string> items)
{
    if (items.size() != 3) {
        throw EntityException("too few items to create a BallState");
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
        throw EntityException("ball position is in invalid format");
    }

    itemPtr++;
    int direction;

    try {
        direction = std::stoi(*itemPtr);

        if (!isValidDirection(direction)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw EntityException("ball direction is in invalid format");
    }

    itemPtr++;
    int speed;

    try {
        speed = std::stoi(*itemPtr);

        if (!isValidPosition(speed)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw EntityException("ball speed is in invalid format");
    }

    this->position = static_cast<int16_t>(position);
    this->direction = static_cast<int8_t>(direction);
    this->speed = static_cast<int16_t>(speed);
}

void BallState::itemize(std::list<std::string> &destination)
{
    destination.clear();
    destination.push_back(std::to_string(position));
    destination.push_back(std::to_string(direction));
    destination.push_back(std::to_string(speed));
}

int16_t BallState::getPosition()
{
    return position;
}

int8_t BallState::getDirection()
{
    return direction;
}

int16_t BallState::getSpeed()
{
    return speed;
}

bool BallState::isValidPosition(int position)
{
    return position <= Game::BALL_POSITION_MAX && position >= Game::BALL_POSITION_MIN;
}

bool BallState::isValidDirection(int direction)
{
    return direction <= Game::BALL_DIRECTION_MAX && direction >= Game::BALL_DIRECTION_MIN;
}

bool BallState::isValidSpeed(int speed)
{
    return speed <= Game::BALL_SPEED_MAX && speed >= Game::BALL_SPEED_MIN;;
}

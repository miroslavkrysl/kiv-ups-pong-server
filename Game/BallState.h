#pragma once

#include <cstdint>
#include <string>
#include <list>
#include "Game.h"

class BallState
{
public:
    static const int16_t RADIUS{25};

    static const int16_t POSITION_MAX{(Game::HEIGHT / 2) - RADIUS};
    static const int16_t POSITION_MIN{-(Game::HEIGHT / 2) + RADIUS};

    static const int8_t DIRECTION_MAX{60};
    static const int8_t DIRECTION_MIN{-60};

    static const int16_t SPEED_MAX{1920};
    static const int16_t SPEED_MIN{640};

private:
    int16_t position;
    int8_t direction;
    int16_t speed;

    explicit BallState(int16_t position = 0, int8_t direction = 0, int16_t speed = SPEED_MIN);
    explicit BallState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    int16_t getPosition();
    int8_t getDirection();
    int16_t getSpeed();

    bool isValidPosition(int position);
    bool isValidDirection(int direction);
    bool isValidSpeed(int speed);
};



#pragma once

#include <cstdint>
#include <string>
#include <list>

class Game;

class BallState
{
    int16_t position;
    int8_t direction;
    int16_t speed;

public:
    explicit BallState();
    explicit BallState(int16_t position, int8_t direction, int16_t speed);
    explicit BallState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    int16_t getPosition();
    int8_t getDirection();
    int16_t getSpeed();

    bool isValidPosition(int position);
    bool isValidDirection(int direction);
    bool isValidSpeed(int speed);
};



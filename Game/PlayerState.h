#pragma once

#include <string>
#include <list>

#include "Game.h"

class PlayerState
{
    enum class Direction
    {
        Up,
        Down,
        Stop
    };

    static const int16_t HEIGHT{320};
    static const int16_t SPEED{400};

    static const int16_t POSITION_MAX{(Game::HEIGHT / 2) - (HEIGHT / 2)};
    static const int16_t POSITION_MIN{-(Game::HEIGHT / 2) + (HEIGHT / 2)};

    int16_t position;
    Direction direction;

public:
    explicit PlayerState(int16_t position = 0, Direction direction = Direction::Stop);
    explicit PlayerState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    int16_t getPosition();
    Direction getDirection();

    bool isValidPosition(int position);
};



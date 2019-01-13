#pragma once

#include <string>
#include <list>

class Game;

class PlayerState
{
    enum class Direction
    {
        Up,
        Down,
        Stop
    };

    int16_t position;
    Direction direction;

public:
    PlayerState();
    explicit PlayerState(int16_t position, Direction direction);
    explicit PlayerState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    int16_t getPosition();
    Direction getDirection();

    bool isValidPosition(int position);
};



#pragma once

#include <cstdint>
#include <string>
#include <list>

#include "GameTypes.h"

class Game;

class BallState
{
    Timestamp timestamp_;
    BallPosition position_;
    BallDirection direction_;
    Speed speed_;

public:
    BallState();
    BallState(Timestamp timestamp, BallPosition position, BallDirection direction, Speed speed);
    explicit BallState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    Timestamp timestamp();
    BallPosition position();
    BallDirection direction();
    Speed speed();
};



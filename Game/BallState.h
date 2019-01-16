#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"

class Game;

class BallState
{
private:
    Timestamp timestamp_;
    Side side_;
    Position position_;
    Angle angle_;
    Speed speed_;

public:
    BallState(Timestamp timestamp, Side side, Position position, Angle direction, Speed speed);

    Timestamp timestamp() const;
    Side side() const;
    Position position() const;
    Angle angle() const;
    Speed speed() const;

    std::vector<std::string> itemize() const;
};



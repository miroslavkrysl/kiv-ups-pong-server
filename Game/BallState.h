#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"

class Game;

class BallState
{
public:
    static const size_t ITEMS_COUNT{5};

private:
    Timestamp timestamp_;
    Side side_;
    BallPosition position_;
    Angle angle_;
    Speed speed_;

    void validate();

public:
    BallState();
    BallState(Timestamp timestamp, Side side, BallPosition position, Angle direction, Speed speed);
    explicit BallState(std::vector<std::string> items);

    Timestamp timestamp();
    Side side();
    BallPosition position();
    Angle angle();
    Speed speed();

    std::vector<std::string> itemize();
};



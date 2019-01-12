#pragma once

#include <cstdint>

#include "Game.h"
#include "Player.h"
#include "Ball.h"

class BallPosition
{
public:
    static const int32_t MAX{(Game::HEIGHT / 2) - Ball::RADIUS};
    static const int32_t MIN{-(Game::HEIGHT / 2) + Ball::RADIUS};
};



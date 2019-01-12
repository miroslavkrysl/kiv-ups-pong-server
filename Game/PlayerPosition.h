#pragma once

#include <cstdint>

#include "Player.h"
#include "Game.h"

class PlayerPosition
{
public:
    static const int32_t MAX{(Game::HEIGHT / 2) - (Player::HEIGHT / 2)};
    static const int32_t MIN{-(Game::HEIGHT / 2) + (Player::HEIGHT / 2)};
};



#pragma once

#include <random>
#include "BallState.h"

class Game
{
public:
    static const int16_t WIDTH{1920};
    static const int16_t HEIGHT{1080};

private:
    std::default_random_engine randomEngine;
    std::uniform_int_distribution<int32_t> randomBallDir;
    std::uniform_int_distribution<int32_t> randomBallSpeed;

    BallState ball;
public:

};



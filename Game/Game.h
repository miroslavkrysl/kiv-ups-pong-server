#pragma once

#include <array>
#include <chrono>
#include <stdexcept>

#include "BallState.h"
#include "PlayerState.h"
#include "GameStateMachine.h"

class Game
{
    GameStateMachine stateMachine;
    BallState ball;
    PlayerState playerLeft;
    PlayerState playerRight;
    std::pair<std::string, std::string> nicknames;
    std::pair<Score, Score> score;

    const std::chrono::steady_clock::time_point startTime;

    PlayerState &getPlayerState_(std::string nickname);

    PlayerState expectedPlayerState(PlayerState &state, Timestamp timestamp);
    BallState expectedBallState(BallState &state);

public:
    Game(std::string playerLeft, std::string playerRight);

    const PlayerState &getPlayerState(std::string nickname);
    const PlayerState &getOpponentState(std::string nickname);
    const BallState &getBallState();
    Side getPlayerSide(std::string nickname);
    Timestamp getTime();

    void updatePlayerState(std::string nickname, PlayerState state);
    void ballHit(std::string nickname, PlayerState state, BallState ballState);
    void ballMiss(std::string nickname, PlayerState state);

    bool isInPast(Timestamp timestamp);
};

class GameException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class GamePlayException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};




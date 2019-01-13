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

    PlayerState *getPlayerStatePtr(std::string nickname);
    PlayerState getNextPlayerState(BallState &state, Timestamp timestamp);
    BallState getNextBallState(BallState &state);

public:
    Game(std::string playerLeft, std::string playerRight);

    PlayerState &getPlayerState(std::string nickname);
    PlayerState &getOpponentState(std::string nickname);
    BallState &getBallState();
    PlayerSide getPlayerSide(std::string nickname);
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




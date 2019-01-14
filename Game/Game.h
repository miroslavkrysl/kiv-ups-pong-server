#pragma once

#include <array>
#include <chrono>
#include <stdexcept>
#include <mutex>

#include "BallState.h"
#include "PlayerState.h"
#include "GameStateMachine.h"

class Connection;

class Game
{
    GameStateMachine stateMachine;
    BallState ballState;
    PlayerState playerLeft;
    PlayerState playerRight;
    Score maxScore;
    Side nextSide;
    std::pair<std::string, std::string> nicknames;
    std::pair<Score, Score> score;
    std::pair<bool, bool> ready;

    const std::chrono::steady_clock::time_point startTime;
    std::mutex mutex;

    PlayerState &getPlayerState_(std::string nickname);

    PlayerState expectedPlayerState(PlayerState &state, Timestamp timestamp);
    BallState expectedBallState(BallState &state);
    bool canHit(std::string nickname, BallState &state);

public:
    Game(std::string playerLeft, std::string playerRight);

    PlayerState getPlayerState(std::string nickname);
    PlayerState getOpponentState(std::string nickname);
    BallState getBallState();
    Side getPlayerSide(std::string nickname);
    std::string getOpponentNickname(std::string nickname);
    Timestamp getTime();
    GameState getState();

    void newRound();
    void playerReady(const std::string &nickname);
    void updatePlayerState(std::string nickname, PlayerState state);
    void ballHit(std::string nickname, BallState ballState);

    bool isInPast(Timestamp timestamp);

    void itemize(std::list<std::string> &destination);
};

class GameException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class GamePlayException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};




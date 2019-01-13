#pragma once

#include <array>
#include <chrono>
#include "BallState.h"
#include "PlayerState.h"
#include "GameStateMachine.h"

enum class PlayerSide {
    Left,
    Right,
};

class Game
{
public:
    static const int16_t GAME_WIDTH{1920};
    static const int16_t GAME_HEIGHT{1080};

    static const int16_t BALL_RADIUS{25};

    static const int16_t BALL_POSITION_MAX{(GAME_HEIGHT / 2) - BALL_RADIUS};
    static const int16_t BALL_POSITION_MIN{-(GAME_HEIGHT / 2) + BALL_RADIUS};

    static const int8_t BALL_DIRECTION_MAX{60};
    static const int8_t BALL_DIRECTION_MIN{-60};

    static const int16_t BALL_SPEED_MAX{1920};
    static const int16_t BALL_SPEED_MIN{640};

    static const int16_t PLAYER_HEIGHT{320};
    static const int16_t PLAYER_SPEED{400};

    static const int16_t PLAYER_POSITION_MAX{(GAME_HEIGHT / 2) - (PLAYER_HEIGHT / 2)};
    static const int16_t PLAYER_POSITION_MIN{-(GAME_HEIGHT / 2) + (PLAYER_HEIGHT / 2)};

    static const int8_t MAX_SCORE{INT8_MAX};

private:
    GameStateMachine stateMachine;
    BallState ball;
    PlayerState playerLeft;
    PlayerState playerRight;
    std::pair<std::string, std::string> nicknames;
    std::pair<uint8_t, uint8_t> score;

    const std::chrono::steady_clock::time_point startTime;

    PlayerState *getPlayerStatePtr(std::string nickname);

public:
    Game(std::string playerLeft, std::string playerRight);

    PlayerState &getPlayerState(std::string nickname);
    PlayerState &getOpponentState(std::string nickname);
    BallState &getBallState();
    PlayerSide getPlayerSide(std::string nickname);
    int32_t getTime();

    void updatePlayerState(std::string nickname, PlayerState state);
    void updateBallState(BallState state);
    void ballHit(std::string nickname);
    void ballMiss(std::string nickname);

    bool isValidTime(unsigned int time);
};

class GameException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class GamePlayException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};




#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

enum class PlayerSide {
    Left,
    Right,
};

enum class PlayerDirection
{
    Up,
    Down,
    Stop
};

typedef uint32_t Timestamp;
typedef int16_t PlayerPosition;
typedef int16_t BallPosition;
typedef uint16_t Size;
typedef uint8_t Score;
typedef uint16_t Speed;
typedef int8_t BallDirection;

const Size GAME_WIDTH{1920};
const Size GAME_HEIGHT{1080};

const Size BALL_RADIUS{25};

const BallPosition BALL_POSITION_MAX{(GAME_HEIGHT / 2) - BALL_RADIUS};
const BallPosition BALL_POSITION_MIN{-(GAME_HEIGHT / 2) + BALL_RADIUS};

const BallDirection BALL_DIRECTION_MAX{60};
const BallDirection BALL_DIRECTION_MIN{-60};

const Speed BALL_SPEED_MAX{1920};
const Speed BALL_SPEED_MIN{640};

const Size PLAYER_HEIGHT{320};

const Speed PLAYER_SPEED{400};

const PlayerPosition PLAYER_POSITION_MAX{(GAME_HEIGHT / 2) - (PLAYER_HEIGHT / 2)};
const PlayerPosition PLAYER_POSITION_MIN{-(GAME_HEIGHT / 2) + (PLAYER_HEIGHT / 2)};

const Score MAX_SCORE{UINT8_MAX};
const Timestamp TIMESTAMP_MAX{UINT32_MAX};

bool isValidTimestamp(unsigned long time);
bool isValidSpeed(unsigned long speed);
bool isValidPlayerPosition(int position);
bool isValidPlayerDirection(PlayerDirection direction);
bool isValidBallPosition(int position);
bool isValidBallDirection(int direction);

Timestamp strToTimestamp(std::string &str);
Speed strToSpeed(std::string &str);
PlayerPosition strToPlayerPosition(std::string &str);
PlayerDirection strToPlayerDirection(std::string &str);
BallPosition strToBallPosition(std::string &str);
BallDirection strToBallDirection(std::string &str);

std::string timestampToStr(Timestamp timestamp);
std::string speedToStr(Speed speed);
std::string scoreToStr(Score score);
std::string playerPositionToStr(PlayerPosition playerPosition);
std::string playerDirectionToStr(PlayerDirection playerDirection);
std::string ballPositionToStr(BallPosition ballPosition);
std::string ballDirectionToStr(BallDirection ballDirection);
std::string playerSideToStr(PlayerSide playerSide);

class GameTypeException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

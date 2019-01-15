#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>

enum class Side {
    Left,
    Right,
};

enum class Direction
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
typedef int8_t Angle;

const Timestamp START_DELAY{3000};

const Size GAME_WIDTH{1920};
const Size GAME_HEIGHT{1080};

const Size BALL_RADIUS{25};

const BallPosition BALL_POSITION_MAX{(GAME_HEIGHT / 2) - BALL_RADIUS};
const BallPosition BALL_POSITION_MIN{-(GAME_HEIGHT / 2) + BALL_RADIUS};

const Angle ANGLE_MAX{60};
const Angle ANGLE_MIN{-60};

const Speed BALL_SPEED_MAX{1920};
const Speed BALL_SPEED_MIN{640};

const Size PLAYER_HEIGHT{320};

const Speed PLAYER_SPEED{400};

const PlayerPosition PLAYER_POSITION_MAX{(GAME_HEIGHT / 2) - (PLAYER_HEIGHT / 2)};
const PlayerPosition PLAYER_POSITION_MIN{-(GAME_HEIGHT / 2) + (PLAYER_HEIGHT / 2)};

const Score MAX_SCORE{UINT8_MAX};
const Timestamp TIMESTAMP_MAX{UINT32_MAX};

bool isValidTimestamp(unsigned long time);
bool isValidSide(Side side);
bool isValidSpeed(unsigned long speed);
bool isValidScore(unsigned long score);
bool isValidPlayerPosition(int position);
bool isValidDirection(Direction direction);
bool isValidBallPosition(int position);
bool isValidAngle(int angle);

Timestamp strToTimestamp(std::string &str);
Side strToSide(std::string &str);
Speed strToSpeed(std::string &str);
Score strToScore(std::string &str);
PlayerPosition strToPlayerPosition(std::string &str);
Direction strToDirection(std::string &str);
BallPosition strToBallPosition(std::string &str);
Angle strToBallAngle(std::string &str);

std::string timestampToStr(Timestamp timestamp);
std::string sideToStr(Side playerSide);
std::string speedToStr(Speed speed);
std::string scoreToStr(Score score);
std::string playerPositionToStr(PlayerPosition position);
std::string directionToStr(Direction direction);
std::string ballPositionToStr(BallPosition position);
std::string angleToStr(Angle angle);

class GameTypeException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

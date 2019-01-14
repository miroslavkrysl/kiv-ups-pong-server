#include "GameTypes.h"

bool isValidTimestamp(unsigned long timestamp)
{
    return timestamp <= TIMESTAMP_MAX;
}

bool isValidSide(Side direction)
{
    return true;
}

bool isValidSpeed(unsigned long speed)
{
    return speed <= BALL_SPEED_MAX && speed >= BALL_SPEED_MIN;;
}

bool isValidScore(unsigned long score)
{
    return score <= MAX_SCORE;
}

bool isValidPlayerPosition(int position)
{
    return position <= PLAYER_POSITION_MAX && position >= PLAYER_POSITION_MIN;
}

bool isValidPlayerDirection(PlayerDirection direction)
{
    return true;
}

bool isValidBallPosition(int position)
{
    return position <= BALL_POSITION_MAX && position >= BALL_POSITION_MIN;
}

bool isValidBallDirection(int direction)
{
    return direction <= BALL_DIRECTION_MAX && direction >= BALL_DIRECTION_MIN;
}

Timestamp strToTimestamp(std::string &str)
{
    unsigned long timestamp;

    try {
        timestamp = std::stoul(str);

        if (!isValidTimestamp(timestamp)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("timestamp is in invalid format");
    }

    return static_cast<Timestamp>(timestamp);
}

Side strToSide(std::string &str)
{
    if (str == "left") {
        return Side::Left;
    }
    else if (str == "right") {
        return Side::Right;
    }
    else {
        throw GameTypeException("side is in invalid format");
    }
}


Speed strToSpeed(std::string &str)
{
    unsigned long speed;

    try {
        speed = std::stoul(str);

        if (!isValidSpeed(speed)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("speed is in invalid format");
    }

    return static_cast<Speed>(speed);
}

Score strToScore(std::string &str)
{
    unsigned long score;

    try {
        score = std::stoul(str);

        if (!isValidSpeed(score)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("score is in invalid format");
    }

    return static_cast<Score>(score);
}

PlayerPosition strToPlayerPosition(std::string &str)
{
    int position;

    try {
        position = std::stoi(str);

        if (!isValidPlayerPosition(position)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("player position is in invalid format");
    }

    return static_cast<PlayerPosition>(position);
}

PlayerDirection strToPlayerDirection(std::string &str)
{
    if (str == "up") {
        return PlayerDirection::Up;
    }
    else if (str == "down") {
        return PlayerDirection::Down;
    }
    else if (str == "stop") {
        return PlayerDirection::Stop;
    }
    else {
        throw GameTypeException("player direction is in invalid format");
    }
}

BallPosition strToBallPosition(std::string &str)
{
    int position;

    try {
        position = std::stoi(str);

        if (!isValidBallPosition(position)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("ball position is in invalid format");
    }

    return static_cast<BallPosition>(position);
}

BallDirection strToBallDirection(std::string &str)
{
    int direction;

    try {
        direction = std::stoi(str);

        if (!isValidBallDirection(direction)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("ball direction is in invalid format");
    }

    return static_cast<BallDirection>(direction);
}

std::string timestampToStr(Timestamp timestamp)
{
    return std::to_string(timestamp);
}

std::string speedToStr(Speed speed)
{
    return std::to_string(speed);
}

std::string scoreToStr(Score score)
{
    return std::to_string(score);
}

std::string playerPositionToStr(PlayerPosition playerPosition)
{
    return std::to_string(playerPosition);
}

std::string playerDirectionToStr(PlayerDirection playerDirection)
{
    switch (playerDirection) {
    case PlayerDirection::Up:return "up";
    case PlayerDirection::Down:return "down";
    case PlayerDirection::Stop:return "stop";
    }
}

std::string ballPositionToStr(BallPosition ballPosition)
{
    return std::to_string(ballPosition);
}

std::string ballDirectionToStr(BallDirection ballDirection)
{
    return std::to_string(ballDirection);
}

std::string sideToStr(Side playerSide)
{
    switch (playerSide) {
    case Side::Left:return "left";
    case Side::Right:return "right";
    }
}

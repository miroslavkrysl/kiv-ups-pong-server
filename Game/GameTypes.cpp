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

bool isValidDirection(Direction direction)
{
    return true;
}

bool isValidBallPosition(int position)
{
    return position <= BALL_POSITION_MAX && position >= BALL_POSITION_MIN;
}

bool isValidAngle(int direction)
{
    return direction <= ANGLE_MAX && direction >= ANGLE_MIN;
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

Direction strToDirection(std::string &str)
{
    if (str == "up") {
        return Direction::Up;
    }
    else if (str == "down") {
        return Direction::Down;
    }
    else if (str == "stop") {
        return Direction::Stop;
    }
    else {
        throw GameTypeException("direction is in invalid format");
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

Angle strToBallAngle(std::string &str)
{
    int direction;

    try {
        direction = std::stoi(str);

        if (!isValidAngle(direction)) {
            throw std::exception{};
        }
    }
    catch (...) {
        throw GameTypeException("angle is in invalid format");
    }

    return static_cast<Angle>(direction);
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

std::string directionToStr(Direction playerDirection)
{
    switch (playerDirection) {
    case Direction::Up:return "up";
    case Direction::Down:return "down";
    case Direction::Stop:return "stop";
    }
}

std::string ballPositionToStr(BallPosition ballPosition)
{
    return std::to_string(ballPosition);
}

std::string angleToStr(Angle ballDirection)
{
    return std::to_string(ballDirection);
}

std::string sideToStr(Side side)
{
    switch (side) {
    case Side::Left:return "left";
    case Side::Center:return "center";
    case Side::Right:return "right";
    }
}

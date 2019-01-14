#include "BallState.h"

BallState::BallState()
    : timestamp_{0},
      side_{Side::Center},
      position_{0},
      direction_{0},
      speed_{BALL_SPEED_MIN}
{}

BallState::BallState(Timestamp timestamp, Side side, BallPosition position, BallDirection direction, Speed speed)
    : timestamp_{timestamp},
      side_{side},
      position_{position},
      direction_{direction},
      speed_{speed}
{
    if (!isValidTimestamp(timestamp)) {
        throw GameTypeException("ball timestamp is invalid");
    }
    if (!isValidSide(side)) {
        throw GameTypeException("ball side is invalid");
    }
    if (!isValidBallPosition(position)) {
        throw GameTypeException("ball position is invalid");
    }
    if (!isValidBallDirection(direction)) {
        throw GameTypeException("ball direction is invalid");
    }
    if (!isValidSpeed(speed)) {
        throw GameTypeException("ball speed is invalid");
    }
}

BallState::BallState(std::list<std::string> items)
{
    if (items.size() != 3) {
        throw GameTypeException("too few items to create a BallState");
    }

    auto itemPtr = items.begin();
    timestamp_ = strToTimestamp(*itemPtr);
    itemPtr++;
    position_ = strToSide(*itemPtr);
    itemPtr++;
    position_ = strToBallPosition(*itemPtr);
    itemPtr++;
    direction_ = strToBallDirection(*itemPtr);
    itemPtr++;
    speed_ = strToSpeed(*itemPtr);
}

void BallState::itemize(std::list<std::string> &destination)
{
    destination.clear();
    destination.push_back(timestampToStr(timestamp_));
    destination.push_back(sideToStr(side_));
    destination.push_back(ballPositionToStr(position_));
    destination.push_back(ballDirectionToStr(direction_));
    destination.push_back(speedToStr(speed_));
}

Timestamp BallState::timestamp()
{
    return timestamp_;
}

Side BallState::side()
{
    return side_;
}

BallPosition BallState::position()
{
    return position_;
}

BallDirection BallState::direction()
{
    return direction_;
}

Speed BallState::speed()
{
    return speed_;
}

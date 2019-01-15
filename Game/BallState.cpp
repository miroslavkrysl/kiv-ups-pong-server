#include "BallState.h"

void BallState::validate()
{
    if (!isValidTimestamp(timestamp_)) {
        throw GameTypeException("ball timestamp is invalid");
    }
    if (!isValidSide(side_)) {
        throw GameTypeException("ball side is invalid");
    }
    if (!isValidBallPosition(position_)) {
        throw GameTypeException("ball position is invalid");
    }
    if (!isValidAngle(angle_)) {
        throw GameTypeException("ball angle is invalid");
    }
    if (!isValidSpeed(speed_)) {
        throw GameTypeException("ball speed is invalid");
    }
}

BallState::BallState()
    : timestamp_{0},
      side_{Side::Center},
      position_{0},
      angle_{0},
      speed_{BALL_SPEED_MIN}
{
    validate();
}

BallState::BallState(Timestamp timestamp, Side side, BallPosition position, Angle angle, Speed speed)
    : timestamp_{timestamp},
      side_{side},
      position_{position},
      angle_{angle},
      speed_{speed}
{
    validate();
}

BallState::BallState(std::vector<std::string> items)
{
    if (items.size() != ITEMS_COUNT) {
        throw GameTypeException("too few items to create a BallState");
    }

    timestamp_ = strToTimestamp(items[0]);
    side_ = strToSide(items[1]);
    position_ = strToBallPosition(items[2]);
    angle_ = strToBallAngle(items[3]);
    speed_ = strToSpeed(items[4]);

    validate();
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

Angle BallState::angle()
{
    return angle_;
}

Speed BallState::speed()
{
    return speed_;
}

std::vector<std::string> BallState::itemize()
{
    return {
        timestampToStr(timestamp_),
        ballPositionToStr(position_),
        angleToStr(angle_),
        speedToStr(speed_)
    };
}

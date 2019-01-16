#include "BallState.h"

BallState::BallState(Timestamp timestamp, Side side, Position position, Angle angle, Speed speed)
    : timestamp_{timestamp},
      side_{side},
      position_{position},
      angle_{angle},
      speed_{speed}
{}

Timestamp BallState::timestamp() const
{
    return timestamp_;
}

Side BallState::side() const
{
    return side_;
}

Position BallState::position() const
{
    return position_;
}

Angle BallState::angle() const
{
    return angle_;
}

Speed BallState::speed() const
{
    return speed_;
}

std::vector<std::string> BallState::itemize() const
{
    return {
        timestampToStr(timestamp_),
        sideToStr(side_),
        ballPositionToStr(position_),
        angleToStr(angle_),
        speedToStr(speed_)
    };
}

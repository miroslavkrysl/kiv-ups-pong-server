#include "PlayerState.h"

void PlayerState::validate()
{
    if (!isValidTimestamp(timestamp_)) {
        throw GameTypeException("player timestamp is invalid");
    }
    if (!isValidPlayerPosition(position_)) {
        throw GameTypeException("player position is invalid");
    }
    if (!isValidDirection(direction_)) {
        throw GameTypeException("player direction is invalid");
    }
}

PlayerState::PlayerState()
    : timestamp_{0},
      position_{0},
      direction_{Direction::Stop}
{
    validate();
}

PlayerState::PlayerState(Timestamp timestamp, PlayerPosition position, Direction direction)
    : timestamp_{timestamp},
      position_{position},
      direction_{direction}
{
    validate();
}

PlayerState::PlayerState(std::vector<std::string> items)
{
    if (items.size() != ITEMS_COUNT) {
        throw GameTypeException("too few items to create a PlayerState");
    }

    timestamp_ = strToTimestamp(items[0]);
    position_ = strToPlayerPosition(items[1]);
    direction_ = strToDirection(items[2]);

    validate();
}

Timestamp PlayerState::timestamp()
{
    return timestamp_;
}

PlayerPosition PlayerState::position()
{
    return position_;
}

Direction PlayerState::direction()
{
    return direction_;
}

std::vector<std::string> PlayerState::itemize()
{
    return {
        timestampToStr(timestamp_),
        playerPositionToStr(position_),
        directionToStr(direction_)
    };
}

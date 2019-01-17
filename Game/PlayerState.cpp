#include "PlayerState.h"
#include "../Exceptions.h"

void PlayerState::validate()
{
    if (!isValidPlayerPosition(position_)) {
        throw GameTypeException("player position is invalid");
    }
}

PlayerState::PlayerState()
    : timestamp_{0},
      position_{0},
      direction_{PlayerDirection::Stop}
{}

PlayerState::PlayerState(Timestamp timestamp, Position position, PlayerDirection direction)
    : timestamp_{timestamp},
      position_{position},
      direction_{direction}
{}

PlayerState::PlayerState(std::vector<std::string> items)
{
    if (items.size() != ITEMS_COUNT) {
        throw GameTypeException("too few items to create a PlayerState");
    }

    timestamp_ = strToTimestamp(items[0]);
    position_ = strToPlayerPosition(items[1]);
    direction_ = strToPlayerDirection(items[2]);

    validate();
}

Timestamp PlayerState::timestamp() const
{
    return timestamp_;
}

Position PlayerState::position() const
{
    return position_;
}

PlayerDirection PlayerState::direction() const
{
    return direction_;
}

std::vector<std::string> PlayerState::itemize() const
{
    return {
        timestampToStr(timestamp_),
        playerPositionToStr(position_),
        directionToStr(direction_)
    };
}

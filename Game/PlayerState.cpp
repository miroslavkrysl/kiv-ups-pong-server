#include <sstream>

#include "PlayerState.h"

PlayerState::PlayerState()
    : timestamp_{0},
      position_{0},
      direction_{PlayerDirection::Stop}
{}

PlayerState::PlayerState(Timestamp timestamp, PlayerPosition position, PlayerDirection direction)
    : timestamp_{timestamp},
      position_{position},
      direction_{direction}
{
    if (!isValidTimestamp(timestamp)) {
        throw GameTypeException("player timestamp is invalid");
    }
    if (!isValidPlayerPosition(position)) {
        throw GameTypeException("player position is invalid");
    }
    if (!isValidPlayerDirection(direction)) {
        throw GameTypeException("player direction is invalid");
    }
}

PlayerState::PlayerState(std::list<std::string> items)
{
    if (items.size() != ITEMS_COUNT) {
        throw GameTypeException("too few items to create a PlayerState");
    }

    auto itemPtr = items.begin();
    timestamp_ = strToTimestamp(*itemPtr);
    itemPtr++;
    position_ = strToPlayerPosition(*itemPtr);
    itemPtr++;
    direction_ = strToPlayerDirection(*itemPtr);
}

void PlayerState::itemize(std::list<std::string> &destination)
{
    destination.clear();
    destination.push_back(timestampToStr(timestamp_));
    destination.push_back(playerPositionToStr(position_));
    destination.push_back(playerDirectionToStr(direction_));
}

Timestamp PlayerState::timestamp()
{
    return timestamp_;
}

PlayerPosition PlayerState::position()
{
    return position_;
}

PlayerDirection PlayerState::direction()
{
    return direction_;
}

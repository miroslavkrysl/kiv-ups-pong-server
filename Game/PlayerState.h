#pragma once

#include <string>
#include <list>

#include "GameTypes.h"

enum class PlayerDirection;

class PlayerState
{
    Timestamp timestamp_;
    PlayerPosition position_;
    PlayerDirection direction_;

public:
    PlayerState();
    PlayerState(Timestamp timestamp, PlayerPosition position, PlayerDirection direction);
    explicit PlayerState(std::list<std::string> items);

    void itemize(std::list<std::string> &destination);

    Timestamp timestamp();
    PlayerPosition position();
    PlayerDirection direction();
};



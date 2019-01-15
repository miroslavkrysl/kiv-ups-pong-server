#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"

enum class Direction;

class PlayerState
{
public:
    static const size_t ITEMS_COUNT{3};

private:
    Timestamp timestamp_;
    PlayerPosition position_;
    Direction direction_;

    void validate();

public:
    PlayerState();
    explicit PlayerState(std::vector<std::string> items);
    PlayerState(Timestamp timestamp, PlayerPosition position, Direction direction);

    Timestamp timestamp();
    PlayerPosition position();
    Direction direction();

    std::vector<std::string> itemize();
};



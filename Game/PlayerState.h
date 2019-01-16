#pragma once

#include <string>
#include <vector>

#include "GameTypes.h"

enum class PlayerDirection;

class PlayerState
{
public:
    static const size_t ITEMS_COUNT{3};

private:
    Timestamp timestamp_;
    Position position_;
    PlayerDirection direction_;

    void validate();

public:
    PlayerState();
    explicit PlayerState(std::vector<std::string> items);
    PlayerState(Timestamp timestamp, Position position, PlayerDirection direction);

    Timestamp timestamp() const;
    Position position() const;
    PlayerDirection direction() const;

    std::vector<std::string> itemize() const;
};



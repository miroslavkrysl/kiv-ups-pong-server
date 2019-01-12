#pragma once

#include "../Serializable.h"

class PlayerDirection: public Serializable
{
public:
    enum class Dir
    {
        Up,
        Down,
        Stop
    };

private:
    Dir direction;

public:
    explicit PlayerDirection(Dir direction);
    void parse(std::string data) override;
    std::string serialize() override;
    void clear() override;
};


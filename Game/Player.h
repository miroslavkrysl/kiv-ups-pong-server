#pragma once

#include "../Network/Connection.h"

class Player
{
    Connection &connection;
    PlayerState state;
    Score score;

public:
    explicit Player(Connection &connection);

    Connection &getConnection();

    const PlayerState &getState();
    void setState(const PlayerState &state);

    Score getScore();
    void setScore(Score score);
};



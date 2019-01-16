#pragma once

#include "../Network/Connection.h"
#include "PlayerState.h"

class Player
{
    Connection *connection;
    PlayerState state;
    std::string nickname;
    Side side;

public:
    Player(Connection *connection, Side side);

    Connection *getConnection();
    void setConnection(Connection *connection);
    std::string getNickname();
    Side getSide();
    const PlayerState &getState();
    void setState(const PlayerState &state);
};



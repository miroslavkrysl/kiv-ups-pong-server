#pragma once

#include "../Network/Connection.h"
#include "PlayerState.h"

class Player
{
    Connection *connection;
    PlayerState state;
    std::string nickname;
    bool ready;
    bool restart;
    Side side;

public:
    Player(Connection *connection, Side side);

    Connection *getConnection();
    void setConnection(Connection *connection);
    bool isActive();
    bool wantsRestart();
    void setRestart(bool restart);
    std::string getNickname();
    Side getSide();
    const PlayerState &getState();
    bool isReady();
    void setReady(bool ready);
    void setState(const PlayerState &state);
};



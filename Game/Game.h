#pragma once

#pragma once

#include <random>

#include "GameTypes.h"
#include "BallState.h"
#include "PlayerState.h"
#include "../Types.h"
#include "../Util/Thread.h"
#include "../Network/Packet.h"

class App;

class Game: public Thread
{
public:
    const std::chrono::seconds DEFAULT_UPDATE_PERIOD{60};
    const std::chrono::milliseconds TIME_THRESHOLD{50};
    const Position POSITION_THRESHOLD{10};

private:
    App &app;
    Uid uid;

    GamePhase gamePhase;
    BallState ballState;
    PlayerState playerStateLeft;
    PlayerState playerStateRight;
    Uid playerUidLeft;
    Uid playerUidRight;

    bool playerReadyLeft;
    bool playerReadyRight;

    Side serviceSide;
    Score maxScore;

    Score scoreLeft;
    Score scoreRight;

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<Angle> randomAngle{ANGLE_MIN, ANGLE_MAX};
    std::uniform_int_distribution<Speed> randomSpeed{BALL_SPEED_MIN, BALL_SPEED_MAX};

    PlayerState expectedPlayerState(const PlayerState &state, Timestamp timestamp);
    BallState nextBallState(BallState &state, bool fromCenter, Side toSide = Side::Left);
    bool canHit(const PlayerState &playerState, const BallState &BallState);
    bool isInPast(Timestamp timestamp);

    Score &getScore(Side side);
    Score &getScore(Uid uid);
    PlayerState &getPlayerState(Side side);
    PlayerState &getPlayerState(Uid uid);
    BallState &getBallState();
    Side getPlayerSide(Uid uid);
    Uid getOpponent(Uid uid);

    void sendPacket(Uid, Packet packet);

public:

    Game(App &app, Uid uid);

    void eventPlayerJoin(Uid uid);
    void eventPlayerReady(Uid uid);
    void eventPlayerUpdate(Uid uid, PlayerState playerState);
    void eventPlayerLeave(Uid uid);
    void eventBallHit(BallState newBallState);
    void eventBallMiss(Side winner);
    void eventPlayerRestart(Uid uid);

    void run() override;
    void after() override;
};



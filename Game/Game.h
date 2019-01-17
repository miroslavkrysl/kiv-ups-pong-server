#pragma once

#include <list>
#include <chrono>
#include <stdexcept>
#include <mutex>
#include <condition_variable>
#include <random>

#include "Events.h"
#include "Player.h"
#include "GameTypes.h"
#include "BallState.h"

class Connection;
class Logger;

typedef std::list<std::unique_ptr<Event>> EventsList;
typedef std::pair<Player *, Packet> Message;

class Game: public Thread
{
    const std::chrono::seconds DEFAULT_UPDATE_PERIOD{10};

    const std::chrono::steady_clock::time_point startTime;
    Logger &logger;
    BallState ballState;
    BallState futureBallState;
    Score maxScore;
    std::pair<Score, Score> score;
    std::pair<Player, Player> players;
    Player *servicePlayer;
    bool isPlaying;

    std::default_random_engine randomGenerator;
    std::uniform_int_distribution<Angle> randomAngle{ANGLE_MIN, ANGLE_MAX};
    std::uniform_int_distribution<Speed> randomSpeed{BALL_SPEED_MIN, BALL_SPEED_MAX};

    EventsList events;
    std::condition_variable eventHappenCV;
    std::mutex eventHappenMutex;
    std::mutex eventPushMutex;

    std::list<Message> packetsToSend;

    std::unique_ptr<EventsList> extractEvents();

    void sendPackets();
    std::chrono::steady_clock::time_point nextUpdateAt();

    void handleEvent(Event *event);
    void handleEvent(EventPlayerJoin event);
    void handleEvent(EventPlayerUpdate event);
    void handleEvent(EventPlayerReady event);
    void handleEvent(EventPlayerLeft event);
    void handleEvent(EventNewRound event);
    void handleEvent(EventStartRound event);
    void handleEvent(EventBall event);
    void handleEvent(EventGameOver event);
    void handleEvent(EventRestart event);
    void handleEvent(EventEndGame event);

public:
    explicit Game(Logger &logger, Score maxScore = 15, Side firstPlayer = Side::Left);

    void pushEvent(std::unique_ptr<Event> event);
    void pushPacket(Player &player, Packet packet);
    void update();

    Timestamp getTime();
    std::pair<Player *, Player *> getPlayers();
    Player &getPlayer(Connection &connection);
    Player &getOpponent(Player &player);
    bool isInPast(Timestamp timestamp);
    bool hasBothPlayers();

    PlayerState expectedPlayerState(const PlayerState &state, Timestamp timestamp);
    BallState nextBallState(BallState &state);
    bool canHit(const PlayerState &playerState, const BallState &BallState);
    bool eventHappen();

    void before() override;
    void run() override;
};

class GameException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

#pragma once

#include "PlayerState.h"

class Connection;
class Player;

enum class EventType
{
    PlayerJoin,
    PlayerReady,
    PlayerUpdate,
    PlayerLeft,
    NewRound,
    StartRound,
    Ball,
    GameOver,
    Restart,
    EndGame,
};

class Event
{
    EventType type;
public:
    explicit Event(EventType type);
    virtual EventType getType();
};

class EventPlayerJoin: public Event
{
public:
    Connection &connection;
    explicit EventPlayerJoin(Connection &connection);
};

class EventPlayerReady: public Event
{
public:
    Player &player;
    explicit EventPlayerReady(Player &player);
};

class EventPlayerUpdate: public Event
{
public:
    Player &player;
    PlayerState state;
    EventPlayerUpdate(Player &player, PlayerState state);
};

class EventPlayerLeft: public Event
{
public:
    Player &player;
    explicit EventPlayerLeft(Player &player);
};

class EventNewRound: public Event
{
public:
    EventNewRound();
};

class EventStartRound: public Event
{
public:
    EventStartRound();
};

class EventBall: public Event
{
public:
    EventBall();
};

class EventGameOver: public Event
{
public:
    EventGameOver();
};

class EventRestart: public Event
{
public:
    Player &player;
    explicit EventRestart(Player &player);
};

class EventEndGame: public Event
{
public:
    EventEndGame();
};

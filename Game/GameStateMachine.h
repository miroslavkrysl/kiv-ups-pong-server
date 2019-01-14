#pragma once

#include "../Utils/StateMachine.h"

enum class GameState {
    NewRound,
    StartLeft,
    StartRight,
    ToLeft,
    ToRight,
    End,
};

enum class GameEvent {
    HitLeft,
    HitRight,
    MissLeft,
    MissRight,
    ReleaseLeft,
    ReleaseRight,
    EndGame,
    NewRound,
};

class GameStateMachine
{
    StateMachine stateMachine;

public:
    GameStateMachine();
    void addState(GameState state);
    void addTransition(GameState startState, GameState endState, GameEvent input);
    void setCurrentState(GameState state);
    GameState getCurrentState();
    void doTransition(GameEvent input);
};



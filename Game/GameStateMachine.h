#pragma once

#include "../Utils/StateMachine.h"

enum class GameState {
    NewRound,
    Ready,
    StartLeft,
    StartRight,
    ToLeft,
    ToRight,
};

enum class GameEvent {
    NewBall,
    HitLeft,
    HitRight,
    MissLeft,
    MissRight,
    ReleaseLeft,
    ReleaseRight,
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



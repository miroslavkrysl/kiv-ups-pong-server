#pragma once

#include "../Utils/StateMachine.h"

enum class GameState {
    NewRound,
    BallReady,
    BallToLeft,
    BallToRight,
};

enum class GameEvent {
    NewBall,
    BallHitLeft,
    BallHitRight,
    BallMissLeft,
    BallMissRight,
    BallStartLeft,
    BallStartRight,
};

class GameStateMachine
{
    StateMachine stateMachine;

public:
    void addState(GameState state);
    void addTransition(GameState startState, GameState endState, GameEvent input);
    void setCurrentState(GameState state);
    GameState getCurrentState();
    void doTransition(GameEvent input);
};



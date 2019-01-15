#pragma once

#include "../Utils/StateMachine.h"

enum class GameState {
    New,
    JoinedL,
    JoinedR,
    RoundStart,
    ReadyL,
    ReadyR,
    ReadyBoth,
    BallToL,
    BallToR,
    LeftR,
    LeftL,
    End,
};

enum class GameEvent {
    JoinL,
    JoinR,
    ReadyL,
    ReadyR,
    ReleaseL,
    ReleaseR,
    HitL,
    HitR,
    MissL,
    MissR,
    LeaveL,
    LeaveR,
    End,
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



#include "GameStateMachine.h"

GameStateMachine::GameStateMachine()
{
    addState(GameState::NewRound);
    addState(GameState::Ready);
    addState(GameState::StartLeft);
    addState(GameState::StartRight);
    addState(GameState::ToLeft);
    addState(GameState::ToRight);

    addTransition(GameState::NewRound, GameState::Ready, GameEvent::NewBall);
    addTransition(GameState::Ready, GameState::StartLeft, GameEvent::ReleaseLeft);
    addTransition(GameState::Ready, GameState::StartRight, GameEvent::ReleaseRight);
    addTransition(GameState::StartLeft, GameState::Ready, GameEvent::NewBall);
    addTransition(GameState::StartRight, GameState::Ready, GameEvent::NewBall);
    addTransition(GameState::StartLeft, GameState::ToRight, GameEvent::HitLeft);
    addTransition(GameState::StartRight, GameState::ToLeft, GameEvent::HitRight);
    addTransition(GameState::StartLeft, GameState::NewRound, GameEvent::MissLeft);
    addTransition(GameState::StartRight, GameState::NewRound, GameEvent::MissRight);
    addTransition(GameState::ToLeft, GameState::ToRight, GameEvent::HitLeft);
    addTransition(GameState::ToRight, GameState::ToLeft, GameEvent::HitRight);
    addTransition(GameState::ToLeft, GameState::NewRound, GameEvent::MissLeft);
    addTransition(GameState::ToRight, GameState::NewRound, GameEvent::MissRight);
    addTransition(GameState::ToLeft, GameState::Ready, GameEvent::NewBall);
    addTransition(GameState::ToRight, GameState::Ready, GameEvent::NewBall);

    setCurrentState(GameState::NewRound);
}

void GameStateMachine::addState(GameState state)
{
    stateMachine.addState(static_cast<int>(state));
}

void GameStateMachine::addTransition(GameState startState, GameState endState, GameEvent input)
{
    stateMachine.addTransition(static_cast<int>(startState), static_cast<int>(endState), static_cast<int>(input));
}

void GameStateMachine::setCurrentState(GameState state)
{
    stateMachine.setCurrentState(static_cast<int>(state));
}

GameState GameStateMachine::getCurrentState()
{
    return static_cast<GameState>(stateMachine.getCurrentState());
}

void GameStateMachine::doTransition(GameEvent input)
{
    stateMachine.doTransition(static_cast<int>(input));
}

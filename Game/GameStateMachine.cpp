#include "GameStateMachine.h"

GameStateMachine::GameStateMachine()
{
    addState(GameState::NewRound);
    addState(GameState::StartLeft);
    addState(GameState::StartRight);
    addState(GameState::ToLeft);
    addState(GameState::ToRight);
    addState(GameState::End);

    addTransition(GameState::NewRound, GameState::StartLeft, GameEvent::ReleaseLeft);
    addTransition(GameState::NewRound, GameState::StartRight, GameEvent::ReleaseRight);
    addTransition(GameState::StartLeft, GameState::NewRound, GameEvent::NewRound);
    addTransition(GameState::StartRight, GameState::NewRound, GameEvent::NewRound);
    addTransition(GameState::StartLeft, GameState::ToRight, GameEvent::HitLeft);
    addTransition(GameState::StartRight, GameState::ToLeft, GameEvent::HitRight);
    addTransition(GameState::StartLeft, GameState::NewRound, GameEvent::MissLeft);
    addTransition(GameState::StartRight, GameState::NewRound, GameEvent::MissRight);
    addTransition(GameState::ToLeft, GameState::ToRight, GameEvent::HitLeft);
    addTransition(GameState::ToRight, GameState::ToLeft, GameEvent::HitRight);
    addTransition(GameState::ToLeft, GameState::NewRound, GameEvent::MissLeft);
    addTransition(GameState::ToRight, GameState::NewRound, GameEvent::MissRight);
    addTransition(GameState::ToLeft, GameState::NewRound, GameEvent::NewRound);
    addTransition(GameState::ToRight, GameState::NewRound, GameEvent::NewRound);
    addTransition(GameState::NewRound, GameState::End, GameEvent::EndGame);

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

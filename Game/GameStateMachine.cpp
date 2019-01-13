#include "GameStateMachine.h"

void GameStateMachine::addState(GameState state)
{
    stateMachine.addState(static_cast<int>(state));
}

void GameStateMachine::addTransition(GameState startState, GameState endState, GameEvent input)
{
    stateMachine.addTransition(static_cast<int>(startState), static_cast<int>(endState), static_cast<int>(input))
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

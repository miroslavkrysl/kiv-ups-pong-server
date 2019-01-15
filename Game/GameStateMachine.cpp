#include "GameStateMachine.h"

GameStateMachine::GameStateMachine()
{
    addState(GameState::New);
    addState(GameState::JoinedL);
    addState(GameState::JoinedR);
    addState(GameState::RoundStart);
    addState(GameState::ReadyL);
    addState(GameState::ReadyR);
    addState(GameState::ReadyBoth);
    addState(GameState::BallToL);
    addState(GameState::BallToR);
    addState(GameState::LeftR);
    addState(GameState::LeftL);
    addState(GameState::End);

    // normal flow
    addTransition(GameState::New, GameState::JoinedL, GameEvent::JoinL);
    addTransition(GameState::New, GameState::JoinedR, GameEvent::JoinR);
    addTransition(GameState::JoinedL, GameState::RoundStart, GameEvent::JoinR);
    addTransition(GameState::JoinedR, GameState::RoundStart, GameEvent::JoinL);
    addTransition(GameState::RoundStart, GameState::ReadyL, GameEvent::ReadyL);
    addTransition(GameState::RoundStart, GameState::ReadyR, GameEvent::ReadyR);
    addTransition(GameState::ReadyL, GameState::ReadyBoth, GameEvent::ReadyR);
    addTransition(GameState::ReadyR, GameState::ReadyBoth, GameEvent::ReadyL);
    addTransition(GameState::ReadyBoth, GameState::BallToL, GameEvent::ReleaseL);
    addTransition(GameState::ReadyBoth, GameState::BallToR, GameEvent::ReleaseR);
    addTransition(GameState::BallToL, GameState::BallToR, GameEvent::HitL);
    addTransition(GameState::BallToR, GameState::BallToL, GameEvent::HitR);
    addTransition(GameState::BallToL, GameState::RoundStart, GameEvent::MissL);
    addTransition(GameState::BallToR, GameState::RoundStart, GameEvent::MissR);

    // player left game
    addTransition(GameState::JoinedL, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::JoinedL, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::JoinedR, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::JoinedR, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::RoundStart, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::RoundStart, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::ReadyL, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::ReadyL, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::ReadyR, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::ReadyR, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::ReadyBoth, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::ReadyBoth, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::BallToL, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::BallToL, GameState::LeftR, GameEvent::LeaveR);
    addTransition(GameState::BallToR, GameState::LeftL, GameEvent::LeaveL);
    addTransition(GameState::BallToR, GameState::LeftR, GameEvent::LeaveR);

    addTransition(GameState::LeftL, GameState::End, GameEvent::LeaveR);
    addTransition(GameState::LeftR, GameState::End, GameEvent::LeaveL);

    addTransition(GameState::New, GameState::End, GameEvent::End);

    setCurrentState(GameState::New);
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

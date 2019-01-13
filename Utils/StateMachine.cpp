#include "StateMachine.h"

StateMachine::StateMachine(std::map<int, std::map<int, int>> transitions, int startState)
{
    for (auto stateTransition : transitions) {
        addState(stateTransition.first);
    }

    for (auto stateTransition : transitions) {
        for (auto inputState : stateTransition.second) {
            addTransition(stateTransition.first, inputState.second, inputState.first);
        }
    }

    setCurrentState(startState);
}

void StateMachine::addState(int state)
{
    auto inserted = transitions.emplace(std::make_pair(state, std::map<int, int>{}));

    if (!inserted.second) {
        throw StateMachineException("state " + std::to_string(state) + " already exists");
    }
}

void StateMachine::addTransition(int startState, int endState, int input)
{
    auto sState = transitions.find(startState);
    if (sState == transitions.end()) {
        throw StateMachineException("state " + std::to_string(startState) + " not exists");
    }

    auto eState = transitions.find(endState);
    if (eState == transitions.end()) {
        throw StateMachineException("state " + std::to_string(endState) + " not exists");
    }

    auto inserted = sState->second.emplace(std::make_pair(input, endState));
    if (!inserted.second) {
        throw StateMachineException("transition " + std::to_string(startState) + " | " + std::to_string(input) + " -> "
                                        + std::to_string(endState) + " already exists");
    }
}

void StateMachine::setCurrentState(int state)
{
    auto s = transitions.find(state);
    if (s == transitions.end()) {
        throw StateMachineException("start state " + std::to_string(state) + " not exists");
    }

    currentState = state;
}

int StateMachine::getCurrentState()
{
    return currentState;
}

void StateMachine::doTransition(int input)
{
    auto currentTransitions = transitions.find(currentState)->second;

    auto transition = currentTransitions.find(input);
    if (transition == currentTransitions.end()) {
        throw StateMachineException(
            "transition from state " + std::to_string(currentState) + " with input " + std::to_string(input)
                + " not exists");
    }

    currentState = transition->second;
}

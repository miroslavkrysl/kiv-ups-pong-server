#include <utility>

#include "StateMachine.h"

StateMachine::StateMachine(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions,
                           std::string startState)
{
    for (auto stateTransition : transitions) {
        addState_(stateTransition.first);
    }

    for (auto stateTransition : transitions) {
        for (auto inputState : stateTransition.second) {
            addTransition_(stateTransition.first, inputState.second, inputState.first);
        }
    }

    setCurrentState_(std::move(startState));
}

void StateMachine::addState_(std::string state)
{
    auto inserted = transitions_.emplace(std::make_pair(state, std::unordered_map<std::string, std::string>{}));

    if (!inserted.second) {
        throw StateMachineException("state " + state + " already exists");
    }
}

void StateMachine::addTransition_(std::string startState, std::string endState, std::string input)
{
    auto sState = transitions_.find(startState);
    if (sState == transitions_.end()) {
        throw StateMachineException("state " + startState + " not exists");
    }

    auto eState = transitions_.find(endState);
    if (eState == transitions_.end()) {
        throw StateMachineException("state " + endState + " not exists");
    }

    auto inserted = sState->second.emplace(std::make_pair(input, endState));
    if (!inserted.second) {
        throw StateMachineException("transition " + startState + " | " + input + " -> " + endState + " already exists");
    }
}

void StateMachine::setCurrentState_(std::string state)
{
    auto s = transitions_.find(state);
    if (s == transitions_.end()) {
        throw StateMachineException("start state " + state + " not exists");
    }

    currentState_ = state;
}

std::string StateMachine::getCurrentState() const
{
    return currentState_;
}

void StateMachine::doTransition(std::string input)
{
    auto currentTransitions = transitions_.find(currentState_)->second;

    auto transition = currentTransitions.find(input);
    if (transition == currentTransitions.end()) {
        throw StateMachineException("transition from state " + currentState_ + " with input " + input + " not exists");
    }

    currentState_ = transition->second;
}

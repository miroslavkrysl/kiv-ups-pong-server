#pragma once

#include <map>

class StateMachine
{
    std::map<int, std::map<int, int>> transitions;
    int currentState{};

public:
    StateMachine() = default;

    virtual int getCurrentState();
    virtual void setCurrentState(int state);
    virtual void addState(int state);
    virtual void addTransition(int startState, int endState, int input);

    virtual void doTransition(int input);
};

class StateMachineException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



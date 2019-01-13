#pragma once

#include <map>

class StateMachineException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class StateMachine
{
    std::map<int, std::map<int, int>> transitions;
    int currentState{};

public:
    StateMachine() = default;
    virtual void addState(int state);

    virtual void addTransition(int startState, int endState, int input);
    virtual void setCurrentState(int state);
    virtual int getCurrentState();
    virtual void doTransition(int input);
};



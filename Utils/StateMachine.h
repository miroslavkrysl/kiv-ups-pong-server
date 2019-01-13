#pragma once

#include <map>

class StateMachineException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class StateMachine
{
    std::map<int, std::map<int, int>> transitions;
    int currentState;
    void setCurrentState(int state);
    void addState(int state);
    void addTransition(int startState, int endState, int input);

public:
    explicit StateMachine(std::map<int, std::map<int, int>> transitions, int startState);
    void doTransition(int input);
    int getCurrentState();
};



#pragma once

#include <unordered_map>

class StateMachineException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class StateMachine
{
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions_;
    std::string currentState_;
    void setCurrentState_(std::string state);
    void addState_(std::string state);
    void addTransition_(std::string startState, std::string endState, std::string input);

public:
    explicit StateMachine(std::unordered_map<std::string, std::unordered_map<std::string, std::string>> transitions,
                          std::string startState);
    void doTransition(std::string input);
    std::string getCurrentState() const;
};



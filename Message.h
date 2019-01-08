#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

class Message
{

public:
    static const char TERMINATION_SYMBOL = '\n';
    static const size_t MAX_SIZE = 1024;

    static Message *parse(std::string contents);

    Message() = delete;
    virtual std::string toLogString() = 0;
    virtual std::string serialize() = 0;
};



#pragma once

#include <string>
#include <stdexcept>

class Serializable
{
public:
    virtual void parse(std::string data) = 0;
    virtual std::string serialize() = 0;
    virtual void clear() = 0;
};

class SerializableException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

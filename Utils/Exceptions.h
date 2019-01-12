#pragma once

#include <stdexcept>

class EntityException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};



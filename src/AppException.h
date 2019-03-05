#pragma once

#include <bits/exception.h>
#include <stdexcept>

/**
 * The base application exception class.
 */
class AppException: public std::runtime_error
{
public:
    /**
     * Initialize a new AppException with the given message.
     *
     * @param message The exception message.
     */
    explicit AppException(const std::string &message);
};



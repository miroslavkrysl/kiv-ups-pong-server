#pragma once

#include <bits/exception.h>
#include <stdexcept>

/**
 * The base application exception class.
 */
class EApp: public std::exception
{
public:
    /**
     * Defaulted destructor.
     */
    ~EApp() override = default;

    /**
     * Return the explanatory message.
     *
     * @return The explanatory message.
     */
    const char *what() const noexcept override;
};



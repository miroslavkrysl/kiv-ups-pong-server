#pragma once

#include <exception>
#include <string>

/**
 * The base application exception class.
 */
class Exception: public std::exception
{
    /**
     * The description of the error.
     */
    std::string description;

    /**
     * The context of the error.
     */
    std::string context;

    /**
     * The solution proposal.
     */
    std::string solution;

public:
    /**
     * Initialize a new Exception.
     *
     * @param description The error description.
     * @param context The error context.
     * @param solution The possible solution.
     */
    explicit Exception(const std::string &description, const std::string &context = "", const std::string &solution = "");

    /**
     * Get the descriptive message of the exception.
     *
     * @return The descriptive message.
     */
    const char *what() const noexcept override;
};

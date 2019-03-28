#pragma once

#include <exception>
#include <string>

/**
 * The base application exception class.
 */
class Exception: public std::exception
{
    /**
     * The error description.
     */
    std::string description;

    /**
     * The exception context.
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
     * @param context The exception context.
     * @param solution The solution proposal.
     */
    explicit Exception(const std::string &description, const std::string &context = "", const std::string &solution = "");

    /**
     * Get the descriptive message of the exception.
     *
     * @return The descriptive message.
     */
    const char *what() const noexcept override;

    /**
     * Get the error description.
     *
     * @return The error description.
     */
    const std::string &getDescription() const;

    /**
     * Get the exception context.
     *
     * @return The exception context.
     */
    const std::string &getContext() const;

    /**
     * Get the solution proposal.
     *
     * @return The solution proposal.
     */
    const std::string &getSolution() const;
};

#pragma once

#include <exception>
#include <string>

namespace pong
{

/**
 * The application base class for exceptions.
 */
class Exception: virtual public std::exception
{
    /**
     * The exception description.
     */
    std::string description;

public:
    /**
     * Initialize a new Exception.
     *
     * @param description The exception description.
     */
    explicit Exception(std::string description);

    /**
     * Get the descriptive message of the exception.
     *
     * @return The descriptive message.
     */
    const char *what() const noexcept override;

    /**
     * Get the nested descriptive messages of the exception
     * and all nested exceptions.
     *
     * @return The nested descriptive messages.
     */
    std::string whatNested() const;
};

}

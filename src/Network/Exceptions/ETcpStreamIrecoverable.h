#pragma once

#include "../../EApp.h"

/**
 * Exception ETcpStream should be thrown when an irrecoverable error occurs while
 * performing action with TcpStream leading to closing the stream.
 */
class ETcpStreamIrecoverable: EApp
{

public:
    /**
     * Return the explanatory message.
     *
     * @return The explanatory message.
     */
    const char *what() const noexcept override;
};

#pragma once

#include "../../EApp.h"

/**
 * The exception EInvalidSocket should be thrown when performing an action
 * on invalid socket file descriptor or disconnected socket.
 */
class EInvalidSocket: EApp
{
    /**
     * The socket file descriptor, that caused the exception.
     */
    int socket;

public:
    /**
     * Initialize a new EInvalidSocket exception.
     *
     * @param socket The socket file descriptor, that caused the exception.
     */
    explicit EInvalidSocket(int socket);

    /**
     * Return the explanatory message.
     *
     * @return The explanatory message.
     */
    const char *what() const noexcept override;
};

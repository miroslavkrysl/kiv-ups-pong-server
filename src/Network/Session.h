#pragma once

#include <string>
#include <memory>
#include <chrono>

#include "../User.h"
#include "../App.h"

/**
 * The class Session represents the session of one user across
 * the multiple consecutive connections.
 */
class Session
{
public:
    /**
     * Initialize a new Session.
     *
     * @param token The sessions token.
     * @param user The user/owner of this session.
     * @param rApp The app which this session belongs to.
     */
    Session(std::string token, User user, App &rApp);

    /**
     * Get the session token.
     *
     * @return The token.
     */
    std::string GetToken();

    /**
     * Get the time when the session was last active.
     *
     * @return The time point.
     */
    std::chrono::system_clock::time_point GetLastActiveAt();

    /**
     * Set the time when the session was last active.
     *
     * @param timePoint The time point.
     */
    void SetLastActiveAt(std::chrono::system_clock::time_point timePoint);

private:
    /**
     *  The sessions unique token.
     *  It's used during the user reconnection.
     */
    std::string mToken;

    /**
     *  The owner of this session.
     */
    User mUser;

    /**
     * The sessions current connection.
     */
    std::shared_ptr<Connection> mpConnection;

    /**
     * The users current game.
     */
    std::shared_ptr<Game> mpGame;

    /**
     * The time the session was last active.
     */
    std::chrono::system_clock::time_point mLastActiveAt;
};



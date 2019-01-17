#include "Player.h"

Player::Player(Connection *connection, Side side)
    : side(side),
      ready(false),
      restart(false)
{
    setConnection(connection);
}

Connection *Player::getConnection()
{
    return connection;
}

void Player::setConnection(Connection *connection)
{
    this->connection = connection;

    if (this->connection) {
        nickname = connection->getNickname();
    }
}

std::string Player::getNickname()
{
    return nickname;
}

Side Player::getSide()
{
    return side;
}

const PlayerState &Player::getState()
{
    return state;
}

void Player::setState(const PlayerState &state)
{
    this->state = state;
}

bool Player::isReady()
{
    return ready;
}

void Player::setReady(bool ready)
{
    this->ready = ready;
}

bool Player::isActive()
{
    return connection != nullptr;
}

bool Player::wantsRestart()
{
    return restart;
}

void Player::setRestart(bool restart)
{
    this->restart = restart;
}

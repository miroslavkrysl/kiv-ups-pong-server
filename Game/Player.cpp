#include "Player.h"

Player::Player(Connection *connection, Side side)
    : side(side)
{
    setConnection(connection);
}

Connection *Player::getConnection()
{
    return nullptr;
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

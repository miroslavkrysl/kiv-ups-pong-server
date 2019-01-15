#include "Player.h"

Player::Player(Connection &connection)
    : connection{connection},
      score{0}
{}

Connection &Player::getConnection()
{
    return connection;
}

const PlayerState &Player::getState()
{
    return state;
}

void Player::setState(const PlayerState &state)
{

}

Score Player::getScore()
{
    return 0;
}
void Player::setScore(Score score)
{

}

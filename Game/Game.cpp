#include <cmath>
#include <CLucene/util/CLStreams.h>
#include "Game.h"

Game::Game(std::string playerLeft, std::string playerRight)
    : nicknames{playerLeft, playerRight},
      score{0, 0}
{
    if (playerLeft == playerRight) {
        throw GameException("players must have distinct nicknames");
    }

    stateMachine.addState(GameState::NewRound);
    stateMachine.addState(GameState::BallReady);
    stateMachine.addState(GameState::BallToLeft);
    stateMachine.addState(GameState::BallToRight);

    stateMachine.addTransition(GameState::NewRound, GameState::BallReady, GameEvent::NewBall);
    stateMachine.addTransition(GameState::BallReady, GameState::BallToLeft, GameEvent::BallStartLeft);
    stateMachine.addTransition(GameState::BallReady, GameState::BallToRight, GameEvent::BallStartRight);
    stateMachine.addTransition(GameState::BallToLeft, GameState::BallToRight, GameEvent::BallHitLeft);
    stateMachine.addTransition(GameState::BallToRight, GameState::BallToLeft, GameEvent::BallHitRight);
    stateMachine.addTransition(GameState::BallToLeft, GameState::NewRound, GameEvent::BallMissLeft);
    stateMachine.addTransition(GameState::BallToRight, GameState::NewRound, GameEvent::BallMissRight);
}

PlayerState &Game::getPlayerState_(std::string nickname)
{
    if (nickname == nicknames.first) {
        return playerLeft;
    }
    else if (nickname == nicknames.second) {
        return playerRight;
    }

    throw GameException("No such player: " + nickname);
}

const PlayerState &Game::getPlayerState(std::string nickname)
{
    return getPlayerState_(std::move(nickname));
}

const PlayerState &Game::getOpponentState(std::string nickname)
{
    if (nickname == nicknames.first) {
        return getPlayerState_(nicknames.second);
    }
    else if (nickname == nicknames.second) {
        return getPlayerState_(nicknames.first);
    }

    throw GameException("No such player: " + nickname);
}

const BallState &Game::getBallState()
{
    return ball;
}

Side Game::getPlayerSide(std::string nickname)
{
    if (nickname == nicknames.first) {
        return Side::Left;
    }
    else if (nickname == nicknames.second) {
        return Side::Right;
    }

    throw GameException("No such player: " + nickname);
}

Timestamp Game::getTime()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<Timestamp, std::milli>
        time = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return time.count();
}

void Game::updatePlayerState(std::string nickname, PlayerState state)
{
    PlayerState &oldState = getPlayerState_(std::move(nickname));
    PlayerState expected = expectedPlayerState(oldState, state.timestamp());

    if (state.position() != expected.position()) {
        throw GamePlayException("new player state does not correspond with the expected!");
    }

    oldState = state;
}

void Game::ballHit(std::string nickname, PlayerState state, BallState ballState)
{
    // TODO: do hit event
}

void Game::ballMiss(std::string nickname, PlayerState state)
{
    // TODO: do miss event
}

bool Game::isInPast(Timestamp time)
{
    return time <= getTime();
}

PlayerState Game::expectedPlayerState(PlayerState &state, Timestamp timestamp)
{
    if (state.timestamp() > timestamp) {
        throw GameException("player state timestamp is in future");
    }

    if (state.direction() == PlayerDirection::Stop) {
        return {timestamp, state.position(), state.direction()};
    }

    double seconds = (timestamp - state.timestamp()) / 1000.0;
    int dir = state.direction() == PlayerDirection::Up ? 1 : -1;

    long position = static_cast<long>(state.position() + (dir * PLAYER_SPEED * seconds));

    if (position > PLAYER_POSITION_MAX) {
        position = PLAYER_POSITION_MAX;
    }
    else if (position < PLAYER_POSITION_MIN) {
        position = PLAYER_POSITION_MIN;
    }

    return {timestamp, static_cast<PlayerPosition>(position), state.direction()};
}

BallState Game::expectedBallState(BallState &state)
{
    double radians = M_PI / 180 * state.direction();

    // TODO: compute ball position position

    double distance = (GAME_WIDTH - 2 * BALL_RADIUS) / std::cos(std::abs(radians));

    double timestamp = distance / (state.speed() / 1000.0);
    timestamp += state.timestamp();

    Side side = state.side() == Side::Left ? Side::Right : Side::Left;

    return {static_cast<Timestamp>(timestamp), side, static_cast<BallPosition>(position), state.direction(), state.speed()};
}

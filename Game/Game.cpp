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

PlayerState *Game::getPlayerStatePtr(std::string nickname)
{
    if (nickname == nicknames.first) {
        return &playerLeft;
    }
    else if (nickname == nicknames.second) {
        return &playerRight;
    }

    throw GameException("No such player: " + nickname);
}

PlayerState &Game::getPlayerState(std::string nickname)
{
    return *getPlayerStatePtr(std::move(nickname));
}

PlayerState &Game::getOpponentState(std::string nickname)
{
    if (nickname == nicknames.first) {
        return getPlayerState(nicknames.second);
    }
    else if (nickname == nicknames.second) {
        return getPlayerState(nicknames.first);
    }

    throw GameException("No such player: " + nickname);
}

BallState &Game::getBallState()
{
    return ball;
}

PlayerSide Game::getPlayerSide(std::string nickname)
{
    if (nickname == nicknames.first) {
        return PlayerSide::Left;
    }
    else if (nickname == nicknames.second) {
        return PlayerSide::Right;
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
    PlayerState oldState = getPlayerState(std::move(nickname));

    // TODO: handle player update
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

PlayerState Game::getNextPlayerState(BallState &state, Timestamp timestamp)
{
    // TODO: implement get next player state
    return PlayerState();
}

BallState Game::getNextBallState(BallState &state)
{
    // TODO: implement get next ball state
    return BallState();
}

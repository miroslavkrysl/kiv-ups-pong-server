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

PlayerState Game::getPlayerState(std::string nickname)
{
    return getPlayerState_(std::move(nickname));
}

PlayerState Game::getOpponentState(std::string nickname)
{
    if (nickname == nicknames.first) {
        return getPlayerState_(nicknames.second);
    }
    else if (nickname == nicknames.second) {
        return getPlayerState_(nicknames.first);
    }

    throw GameException("No such player: " + nickname);
}

BallState Game::getBallState()
{
    std::lock_guard<std::mutex> lock{mutex};
    return ballState;
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

    if (!isInPast(state.timestamp())) {
        throw GamePlayException("can not set future player state");
    }

    if (state.position() != expected.position()) {
        throw GamePlayException("new player state does not correspond with the expected");
    }

    oldState = state;
}

void Game::ballHit(std::string nickname, BallState ballState)
{
    std::lock_guard<std::mutex> lock{mutex};

    BallState ballStateExpected = expectedBallState(ballState);
    Side playerSide = getPlayerSide(nickname);

    if (ballState.position() != ballStateExpected.position()
        || ballState.timestamp() != ballStateExpected.timestamp()) {
        throw GamePlayException("new ball state does not correspond with the expected");
    }

    if (canHit(nickname, ballState)) {
        if (playerSide == Side::Left) {
            stateMachine.doTransition(GameEvent::HitLeft);
        }
        else {
            stateMachine.doTransition(GameEvent::HitRight);
        }
    }
    else {
        if (playerSide == Side::Left) {
            stateMachine.doTransition(GameEvent::MissLeft);
        }
        else {
            stateMachine.doTransition(GameEvent::MissRight);
        }
    }

    this->ballState = ballState;
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

    double seconds = (timestamp - state.timestamp()) / 1000.0;

    int dir;
    switch (state.direction()) {
    case PlayerDirection::Up: dir = 1; break;
    case PlayerDirection::Stop: dir = 0; break;
    case PlayerDirection::Down: dir = -1; break;
    }

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
    std::lock_guard<std::mutex> lock{mutex};

    double radians = M_PI / 180 * state.direction();
    double width =
        getState() == GameState::StartLeft || getState() == GameState::StartRight
        ? (GAME_WIDTH / 2 - BALL_RADIUS)
        : (GAME_WIDTH - 2 * BALL_RADIUS);
    long height = (GAME_HEIGHT - 2 * BALL_RADIUS);

    double hypotenuse = width / std::cos(std::abs(radians));
    long hLeg = static_cast<long>(std::tan(radians) * width);

    double timestamp = hypotenuse / (state.speed() / 1000.0);
    timestamp += state.timestamp();

    long position = hLeg % height;
    if ((position / height) % 2 == 0) {
        position *= -1;
    }

    position += state.position();

    return {static_cast<Timestamp>(timestamp), static_cast<BallPosition>(position), state.direction(), state.speed()};
}

bool Game::canHit(std::string nickname, BallState &ballState)
{
    Side playerSide = getPlayerSide(nickname);
    PlayerState &playerState = getPlayerState_(nickname);

    return ballState.position() <= playerState.position() + (PLAYER_HEIGHT / 2)
    && ballState.position() >= playerState.position() - (PLAYER_HEIGHT / 2);

}

void Game::newBall()
{
    ballState = BallState{getTime(), 0, 0, 0};
    stateMachine.doTransition(GameEvent::NewBall);
}

void Game::releaseBall(Side side)
{
    ballState = BallState{getTime(), 0, 0, BALL_SPEED_MIN};

    if (side == Side::Left){
        stateMachine.doTransition(GameEvent::ReleaseLeft);
    }
    else {
        stateMachine.doTransition(GameEvent::ReleaseRight);
    }
}

GameState Game::getState()
{
    return stateMachine.getCurrentState();
}

#include "Game.h"
#include "../App.h"
#include "../Exceptions.h"

Game::Game(App &app, Uid uid)
    : app(app),
      uid(uid),
      scoreLeft(0),
      scoreRight(0),
      playerUidLeft(-1),
      playerUidRight(-1),
      playerReadyLeft(false),
      playerReadyRight(false),
      maxScore{15},
      serviceSide{Side::Left},
      gamePhase{GamePhase::New}
{}

PlayerState Game::expectedPlayerState(const PlayerState &state, Timestamp timestamp)
{
    if (state.timestamp() > timestamp) {
        throw ImpossiblePlayerStateException("player state timestamp is in future");
    }

    double seconds = (timestamp - state.timestamp()) / 1000.0;

    int dir;
    switch (state.direction()) {
    case PlayerDirection::Up: dir = 1;
        break;
    case PlayerDirection::Stop: dir = 0;
        break;
    case PlayerDirection::Down: dir = -1;
        break;
    }

    auto position = static_cast<long>(state.position() + (dir * PLAYER_SPEED * seconds));

    if (position > PLAYER_POSITION_MAX) {
        position = PLAYER_POSITION_MAX;
    } else if (position < PLAYER_POSITION_MIN) {
        position = PLAYER_POSITION_MIN;
    }

    return {timestamp, static_cast<Position>(position), state.direction()};
}

BallState Game::nextBallState(BallState &state, bool fromCenter, Side toSide)
{
    double radians = M_PI / 180 * state.angle();
    double width =
        fromCenter
        ? (GAME_WIDTH / 2 - BALL_RADIUS)
        : (GAME_WIDTH - 2 * BALL_RADIUS);
    long height = (GAME_HEIGHT - 2 * BALL_RADIUS);

    double hypotenuse = width / std::cos(std::abs(radians));
    auto hLeg = static_cast<long>(std::tan(radians) * width);

    double timestamp = hypotenuse / (state.speed() / 1000.0);
    timestamp += state.timestamp();

    long position = hLeg % height;
    if ((position / height) % 2 == 0) {
        position *= -1;
    }

    position += state.position();

    return {
        static_cast<Timestamp>(timestamp),
        fromCenter
        ? toSide
        : state.side() == Side::Left ? Side::Right : Side::Left,
        static_cast<Position>(position),
        randomAngle(randomGenerator),
        randomSpeed(randomGenerator)};
}

bool Game::canHit(const PlayerState &playerState, const BallState &ballState)
{
    return ballState.position() <= playerState.position() + (PLAYER_HEIGHT / 2)
        && ballState.position() >= playerState.position() - (PLAYER_HEIGHT / 2);
}

bool Game::isInPast(Timestamp timestamp)
{
    return timestamp <= app.getCurrentTimestamp();
}

Score &Game::getScore(Side side)
{
    switch (side) {
    case Side::Left: { return scoreLeft; }
    case Side::Right: { return scoreRight; }
    }
}

Score &Game::getScore(Uid uid)
{
    return getScore(getPlayerSide(uid));
}

PlayerState &Game::getPlayerState(Side side)
{
    switch (side) {
    case Side::Left: { return playerStateLeft; }
    case Side::Right: { return playerStateRight; }
    }
}

PlayerState &Game::getPlayerState(Uid uid)
{
    return getPlayerState(getPlayerSide(uid));
}

BallState &Game::getBallState()
{
    return ballState;
}

Side Game::getPlayerSide(Uid uid)
{
    if (playerUidLeft == uid) {
        return Side::Left;
    } else if (playerUidRight == uid) {
        return Side::Right;
    }

    throw GameException("player with uid " + std::to_string(uid) + " is not in this game");
}

Uid Game::getOpponent(Uid uid)
{
    switch (getPlayerSide(uid)) {
    case Side::Left: { return playerUidRight; }
    case Side::Right: { return playerUidLeft; }
    }
}

void Game::sendPacket(Uid uid, Packet packet)
{
    app.getPacketHandler().handleOutgoingPacket(uid, packet);
}

Uid Game::getUid()
{
    return uid;
}

void Game::eventPlayerJoin(Uid uid)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::New) {
        throw GameException("can not join more players");
    }

    if (uid < 0) {
        throw GameException("invalid player uid");
    }

    if (uid == playerUidLeft != uid == playerUidRight) {
        throw AlreadyInGameException("player " + std::to_string(uid) + " is already in game " + std::to_string(this->uid));
    }

    Packet packetJoined{"joined"};

    if (playerUidLeft == -1) {

        playerUidLeft = uid;
        packetJoined.addItem(sideToStr(Side::Left));

        sendPacket(playerUidLeft, packetJoined);

    } else if (playerUidRight == -1) {

        playerUidRight = uid;
        packetJoined.addItem(sideToStr(Side::Right));

        sendPacket(playerUidRight, packetJoined);

        sendPacket(playerUidRight, Packet{"opponent_joined", {app.getNickname(playerUidLeft)}});
        sendPacket(playerUidLeft, Packet{"opponent_joined", {app.getNickname(playerUidRight)}});

        Packet packetNewRound{"new_round"};
        packetNewRound.addItem(scoreToStr(scoreLeft));
        packetNewRound.addItem(scoreToStr(scoreRight));

        sendPacket(playerUidLeft, packetNewRound);
        sendPacket(playerUidRight, packetNewRound);

        gamePhase = GamePhase::Waiting;
    }
}

void Game::eventPlayerReady(Uid uid)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::Waiting) {
        throw GamePhaseException("can not accept ready in playing phase");
    }

    Packet packetOpponentReady{"opponent_ready"};

    switch (getPlayerSide(uid)) {
    case Side::Left: {
        playerReadyLeft = true;
        sendPacket(playerUidRight, packetOpponentReady);
        break;
    }
    case Side::Right: {
        playerReadyRight = true;
        sendPacket(playerUidLeft, std::move(packetOpponentReady));
        break;
    }
    }

    if (playerReadyLeft && playerReadyRight) {
        gamePhase = GamePhase::Playing;

        ballState = BallState{
            app.getCurrentTimestamp() + START_DELAY,
            serviceSide,
            0,
            0,
            BALL_SPEED_MIN};

        Packet packetBallReleased{"ball_released", ballState.itemize()};

        sendPacket(playerUidLeft, packetBallReleased);
        sendPacket(playerUidRight, packetBallReleased);

        notifyOne();
        return;
    }
}

void Game::eventPlayerUpdate(Uid uid, PlayerState newPlayerState)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::Playing && gamePhase != GamePhase::Waiting) {
        throw GamePhaseException("can not update player while not playing");
    }

    if (isInPast(newPlayerState.timestamp())) {

        PlayerState &currentState = getPlayerState(uid);
        PlayerState expected = expectedPlayerState(currentState, newPlayerState.timestamp());

        if (std::abs(newPlayerState.position() - expected.position()) < POSITION_THRESHOLD) {
            currentState = newPlayerState;
        }
    }

    sendPacket(uid, Packet{"your_state", getPlayerState(uid).itemize()});
    sendPacket(getOpponent(uid), Packet{"opponent_state", getPlayerState(uid).itemize()});
}

void Game::eventPlayerLeave(Uid uid)
{
    auto lock = acquireLock();

    sendPacket(getOpponent(uid), Packet{"opponent_left"});
    sendPacket(uid, Packet{"left"});

    playerUidLeft = -1;
    playerUidRight = -1;

    gamePhase = GamePhase::End;
    notifyOne();

    stop(false);
}

void Game::eventBallHit(BallState newBallState)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::Playing) {
        throw GamePhaseException("can not hit ball while not playing");
    }

    Packet packet{"ball_hit", ballState.itemize()};

    sendPacket(playerUidLeft, packet);
    sendPacket(playerUidRight, packet);

    ballState = newBallState;
}

void Game::eventBallMiss(Side winner)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::Playing) {
        throw GamePhaseException("can not miss ball while not playing");
    }

    getScore(winner)++;
    playerReadyLeft = false;
    playerReadyRight = false;
    serviceSide = winner;

    if (scoreLeft == maxScore || scoreRight == maxScore) {
        gamePhase = GamePhase::GameOver;
        Packet packet{"game_over"};

        packet.addItem(scoreToStr(scoreLeft));
        packet.addItem(scoreToStr(scoreRight));

        sendPacket(playerUidLeft, packet);
        sendPacket(playerUidRight, packet);
        return;
    }

    gamePhase = GamePhase::Waiting;

    Packet packet{"new_round"};
    packet.addItem(scoreToStr(scoreLeft));
    packet.addItem(scoreToStr(scoreRight));

    sendPacket(playerUidLeft, packet);
    sendPacket(playerUidRight, packet);
}

void Game::eventPlayerRestart(Uid uid)
{
    auto lock = acquireLock();

    if (gamePhase != GamePhase::GameOver) {
        throw GamePhaseException("can not accept ready in playing phase");
    }

    Packet packetOpponentReady{"opponent_ready"};

    switch (getPlayerSide(uid)) {
    case Side::Left: {
        playerReadyLeft = true;
        sendPacket(playerUidRight, packetOpponentReady);
        break;
    }
    case Side::Right: {
        playerReadyRight = true;
        sendPacket(playerUidLeft, packetOpponentReady);
        break;
    }
    }

    if (playerReadyLeft && playerReadyRight) {
        gamePhase = GamePhase::Waiting;
        playerReadyLeft = false;
        playerReadyRight = false;
        scoreLeft = 0;
        scoreRight = 0;

        Packet packetNewRound{"new_round"};
        packetNewRound.addItem(scoreToStr(scoreLeft));
        packetNewRound.addItem(scoreToStr(scoreRight));

        sendPacket(playerUidLeft, packetNewRound);
        sendPacket(playerUidRight, packetNewRound);
    }
}

void Game::run()
{
    // game loop

    while (!shouldStop()) {

        auto nextUpdateAt = std::chrono::system_clock::now();

        if (gamePhase == GamePhase::Playing) {
            // check whether the ball reached the side
            if ((ballState.timestamp() - app.getCurrentTimestamp()) < 0 + TIME_THRESHOLD.count()) {

                // get player on turn
                PlayerState playerState = getPlayerState(ballState.side());
                PlayerState expectedState = expectedPlayerState(playerState, app.getCurrentTimestamp());

                // calculate hit
                if (canHit(expectedState, ballState)) {
                    eventBallHit(nextBallState(ballState, false));
                } else {
                    eventBallMiss(ballState.side());
                }
            }

            nextUpdateAt += std::chrono::milliseconds{ballState.timestamp() - app.getCurrentTimestamp()};
        } else {
            nextUpdateAt += std::chrono::seconds{10};
        }

        auto lock = acquireLock();
        waitUntil(lock, nextUpdateAt, [this] { return gamePhase == GamePhase::Playing; });
    }

}

void Game::after()
{
    Packet packet{"game_ended"};
    sendPacket(playerUidLeft, packet);
    sendPacket(playerUidRight, packet);

    app.notifyOne();
}

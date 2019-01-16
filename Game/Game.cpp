#include <math.h>
#include "Game.h"

void Game::before()
{

}

void Game::run()
{
    std::unique_lock<std::mutex> lockEventHappen(eventHappenMutex, std::defer_lock);

    while (!shouldStop()) {

        update();

        for (auto &event : *extractEvents()) {
            handleEvent(event.get());
        }

        sendPackets();

        lockEventHappen.lock();
        eventHappenCV.wait_until(lockEventHappen, nextUpdateAt(), [this]{ return eventHappen();});
    }
}

void Game::after()
{
    Thread::after();
}

void Game::pushEvent(std::unique_ptr<Event> event)
{
    std::unique_lock<std::mutex> lockPush(eventPushMutex);

    events.push_back(std::move(event));

    eventHappenCV.notify_one();
}

std::unique_ptr<std::list<std::unique_ptr<Event>>> Game::extractEvents()
{
    std::unique_lock<std::mutex> lock(eventPushMutex);

    auto extracted = std::make_unique<EventsList>();
    events.swap(*extracted);

    return extracted;
}

bool Game::eventHappen()
{
    std::unique_lock<std::mutex> lock(eventPushMutex);
    return !events.empty();
}

Game::Game(Score maxScore, Side firstPlayer)
    : maxScore{maxScore},
      players{std::make_pair(Player{nullptr, Side::Left}, Player{nullptr, Side::Right})},
      servicePlayer{players.first},
      startTime{std::chrono::steady_clock::now()}
{}

PlayerState Game::expectedPlayerState(const PlayerState &state, Timestamp timestamp)
{
    if (state.timestamp() > timestamp) {
        throw GameException("player state timestamp is in future");
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
    }
    else if (position < PLAYER_POSITION_MIN) {
        position = PLAYER_POSITION_MIN;
    }

    return {timestamp, static_cast<Position>(position), state.direction()};
}

BallState Game::nextBallState(BallState &state)
{
    double radians = M_PI / 180 * state.angle();
    double width =
        state.side() == Side::CenterToLeft || state.side() == Side::CenterToRight
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
        state.side() == Side::CenterToLeft || state.side() == Side::Right
        ? Side::Left
        : Side::Right,
        static_cast<Position>(position),
        randomAngle(randomGenerator),
        randomSpeed(randomGenerator)};
}

bool Game::canHit(const PlayerState &playerState, const BallState &BallState)
{
    return ballState.position() <= playerState.position() + (PLAYER_HEIGHT / 2)
        && ballState.position() >= playerState.position() - (PLAYER_HEIGHT / 2);
}

Timestamp Game::getTime()
{
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<Timestamp, std::milli>
        time = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
    return time.count();
}

bool Game::isInPast(Timestamp timestamp)
{
    return timestamp <= getTime();
}

void Game::update()
{
    if (!isPlaying) {
        return;
    }

    if (futureBallState.timestamp() <= getTime()) {
        pushEvent(std::make_unique<EventBall>());
    }
}

void Game::sendPackets()
{
    for (auto &packet : packetsToSend) {
        Connection *connection = packet.first->getConnection();

        if (connection) {
            connection->send(packet.second);
        }
    }

    packetsToSend.clear();
}

std::pair<Player *, Player *> Game::getPlayers()
{
    return std::make_pair(&players.first, &players.second);
}

void Game::pushPacket(Player &player, Packet packet)
{
    packetsToSend.emplace_back(&player, std::move(packet));
}

void Game::handleEvent(Event *event)
{
    switch (event->getType()) {
    case EventType::PlayerJoin: {
        handleEvent(*dynamic_cast<EventPlayerJoin *>(event));
    }
    case EventType::PlayerReady: {
        handleEvent(*dynamic_cast<EventPlayerReady *>(event));
    }
    case EventType::PlayerUpdate: {
        handleEvent(*dynamic_cast<EventPlayerUpdate *>(event));
    }
    case EventType::PlayerLeft: {
        handleEvent(*dynamic_cast<EventPlayerLeft *>(event));
    }
    case EventType::NewRound: {
        handleEvent(*dynamic_cast<EventNewRound *>(event));
    }
    case EventType::StartRound: {
        handleEvent(*dynamic_cast<EventStartRound *>(event));
    }
    case EventType::Ball: {
        handleEvent(*dynamic_cast<EventBall *>(event));
    }
    case EventType::GameOver: {
        handleEvent(*dynamic_cast<EventGameOver *>(event));
    }
    case EventType::Restart: {
        handleEvent(*dynamic_cast<EventRestart *>(event));
    }
    case EventType::EndGame: {
        handleEvent(*dynamic_cast<EventEndGame *>(event));
    }
    }
}

void Game::handleEvent(EventPlayerJoin event)
{
    Packet packet1{"joined"};
    Packet packet2{"opponent_joined"};

    if (!players.first.isActive()) {
        players.first.setConnection(&event.connection);
        packet1.addItem(sideToStr(Side::Left));

        pushPacket(players.first, packet1);
        pushPacket(players.second, packet2);
    }
    if (!players.second.isActive()) {
        players.first.setConnection(&event.connection);
        packet1.addItem(sideToStr(Side::Left));

        pushPacket(players.second, packet1);
        pushPacket(players.first, packet2);
    }
}

void Game::handleEvent(EventPlayerUpdate event)
{
    PlayerState currentState = event.player.getState();

    if (!isInPast(event.state.timestamp())) {
        Packet packet{"your_state", currentState.itemize()};
        pushPacket(event.player, packet);
        return;
    }

    PlayerState expected = expectedPlayerState(currentState, event.state.timestamp());

    if (event.state.position() != expected.position()) {
        Packet packet{"your_state", currentState.itemize()};
        pushPacket(event.player, packet);
        return;
    }

    event.player.setState(event.state);


    Packet packet{"opponent_state", event.player.getState().itemize()};
    pushPacket(getOpponent(event.player), packet);
}

void Game::handleEvent(EventPlayerReady event)
{
    event.player.setReady(true);

    Packet packet{"opponent_ready"};
    pushPacket(getOpponent(event.player), packet);

    if (players.first.isReady() || players.second.isReady()) {
        pushEvent(std::make_unique<EventStartRound>());
    }
}

void Game::handleEvent(EventPlayerLeft event)
{
    Packet packet1{"left"};
    Packet packet2{"opponent_left"};

    pushPacket(event.player, packet1);
    pushPacket(getOpponent(event.player), packet2);

    pushEvent(std::make_unique<EventEndGame>());
}

void Game::handleEvent(EventBall event)
{
    isPlaying = false;

    if (ballState.side() == Side::Left) {
        if (!canHit(players.first.getState(), ballState)) {
            score.second += 1;
            servicePlayer = players.second;

            if (score.second == maxScore) {
                pushEvent(std::make_unique<EventGameOver>());
                return;
            }

            pushEvent(std::make_unique<EventNewRound>());
            return;
        }
    }
    else {
        if (!canHit(players.second.getState(), ballState)) {
            score.first += 1;
            servicePlayer = players.first;

            if (score.first == maxScore) {
                pushEvent(std::make_unique<EventGameOver>());
                return;
            }

            pushEvent(std::make_unique<EventNewRound>());
            return;
        }
    }

    isPlaying = true;

    ballState = futureBallState;
    futureBallState = nextBallState(ballState);

    Packet packet{"ball", ballState.itemize()};
    pushPacket(players.first, packet);
    pushPacket(players.second, packet);
}

void Game::handleEvent(EventNewRound event)
{
    isPlaying = false;
    Packet packet{"new_round"};
    packet.addItem(timestampToStr(getTime()));
    packet.addItem(scoreToStr(score.first));
    packet.addItem(scoreToStr(score.second));

    pushPacket(players.first, packet);
    pushPacket(players.second, packet);
}

void Game::handleEvent(EventGameOver event)
{
    isPlaying = false;
    Packet packet{"game_over"};
    packet.addItem(scoreToStr(score.first));
    packet.addItem(scoreToStr(score.second));

    pushPacket(players.first, packet);
    pushPacket(players.second, packet);
}

void Game::handleEvent(EventEndGame event)
{
    players.first.setConnection(nullptr);
    players.second.setConnection(nullptr);

    stop(false);
}

Player &Game::getOpponent(Player &player)
{
    if (players.first.getSide() == player.getSide()) {
        return players.second;
    }
    else if (players.second.getSide() != player.getSide()) {
        return players.first;
    }

    throw GameException("Player " + player.getNickname() + " is not in this game");
}

void Game::handleEvent(EventStartRound event)
{
    ballState = BallState{
        getTime() + START_DELAY,
        servicePlayer.getSide() == Side::Left ? Side::CenterToLeft : Side::CenterToRight,
        0,
        0,
        BALL_SPEED_MIN
    };

    futureBallState = nextBallState(ballState);
    isPlaying = true;

    Packet packet{"start", ballState.itemize()};
    pushPacket(players.first, packet);
    pushPacket(players.second, packet);
}

void Game::handleEvent(EventRestart event)
{
    event.player.setRestart(true);

    if (players.first.wantsRestart() || players.second.wantsRestart()) {
        score.first = 0;
        score.second = 0;

        players.first.setRestart(false);
        players.second.setRestart(false);

        pushEvent(std::make_unique<EventNewRound>());
    }

    Packet packet{"opponent_wants_restart"};
    pushPacket(getOpponent(event.player), packet);
}

std::chrono::steady_clock::time_point Game::nextUpdateAt()
{
    auto now = std::chrono::steady_clock::now();

    if (isPlaying) {
        return now + std::chrono::milliseconds{futureBallState.timestamp() - getTime()};
    }

    return now + DEFAULT_UPDATE_PERIOD;
}

bool Game::hasBothPlayers()
{
    return players.first.isActive() && players.second.isActive();
}

Player &Game::getPlayer(Connection &connection)
{
    if (players.first.getConnection() && players.first.getConnection()->getUid() == connection.getUid()) {
        return players.first;
    }
    else if (players.first.getConnection() && players.first.getConnection()->getUid() == connection.getUid()) {
        return players.second;
    }

    throw GameException("Player " + connection.getNickname() + " is not in this game");
}


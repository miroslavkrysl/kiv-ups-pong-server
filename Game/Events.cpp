#include "Events.h"

Event::Event(EventType type)
    : type(type)
{}

EventType Event::getType()
{
    return type;
}

EventPlayerJoin::EventPlayerJoin(Connection &connection)
    : connection(connection),
      Event(EventType::PlayerJoin)
{}

EventPlayerReady::EventPlayerReady(Player &player)
    : player(player),
      Event(EventType::PlayerReady)
{}

EventPlayerUpdate::EventPlayerUpdate(Player &player, PlayerState state)
    : player(player),
      state(state),
      Event(EventType::PlayerUpdate)
{}

EventPlayerLeft::EventPlayerLeft(Player &player)
    : player(player),
      Event(EventType::PlayerLeft)
{}

EventNewRound::EventNewRound()
    : Event(EventType::NewRound)
{}

EventStartRound::EventStartRound()
    : Event(EventType::StartRound)
{}

EventBall::EventBall()
    : Event(EventType::Ball)
{}

EventRestart::EventRestart(Player &player)
    : player(player),
      Event(EventType::Restart)
{}

EventGameOver::EventGameOver()
    : Event(EventType::GameOver)
{}

EventEndGame::EventEndGame()
    : Event(EventType::EndGame)
{}

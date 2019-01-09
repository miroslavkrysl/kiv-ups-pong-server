#pragma once

#include <string>
#include <functional>
#include <memory>

enum class MessageOrigin
{
    Server,
    Client,
};

class Message
{
    MessageOrigin origin;
    std::string type;

public:

    static const char TERMINATION_SYMBOL = '\n';
    static const size_t MAX_SIZE = 1024;

    static std::unique_ptr<Message> parse(std::string contents);

    Message(MessageOrigin origin, std::string type);

    std::string getType();
    MessageOrigin getOrigin();

    std::string toLogString();
    virtual std::string serialize() = 0;
};

class MessageException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/*
 * Client messages
 */

class LoginMessage: Message
{
public:
    const std::string nickname;
    explicit LoginMessage(std::string nickname);
};

class JoinPrivateGameMessage: Message
{
public:
    const std::string opponent;
    explicit JoinPrivateGameMessage(std::string opponent);
};

class JoinRandomGameMessage: Message
{
public:
    JoinRandomGameMessage();
};

class LeaveGameMessage: Message
{
public:
    LeaveGameMessage();
};

class PlayerStateMessage: Message
{
public:
    const int32_t time;
    const int16_t playerPos;
    const int8_t playerDir;
    PlayerStateMessage(int32_t time, int16_t playerPos, int8_t playerDir);
};

class BallHitMessage: Message
{
public:
    const int32_t time;
    const int16_t ballPos;
    const int16_t ballDir;
    const int16_t ballSpeed;
    BallHitMessage(int32_t time, int16_t ballPos, int16_t ballDir, int16_t ballSpeed);
};

class BallOutMessage: Message
{
public:
    BallOutMessage();
};

class StartNewGameMessage: Message
{
public:
    StartNewGameMessage();
};

class GetTimeMessage: Message
{
public:
    GetTimeMessage();
};

/*
 * Server messages
 */

class LoginOkMessage: Message
{
public:
    LoginOkMessage();
};

class LoginFailedMessage: Message
{
public:
    LoginFailedMessage();
};

class GameJoinedMessage: Message
{
public:
    const std::string opponent;
    explicit GameJoinedMessage(std::string opponent);
};

class GameNotJoinedMessage: Message
{
public:
    GameNotJoinedMessage();
};

class GamePausedMessage: Message
{
public:
    const int32_t time;
    explicit GamePausedMessage(int32_t time);
};

class OpponentLeavedMessage: Message
{
public:
    OpponentLeavedMessage();
};

class OpponentStateMessage: Message
{
public:
    const int32_t time;
    const int16_t opponentPos;
    const int8_t opponentDir;
    OpponentStateMessage(int32_t time, int16_t opponentPos, int8_t opponentDir);
};

class BallStateMessage: Message
{
public:
    const int32_t time;
    const int16_t ballPos;
    const int16_t ballDir;
    const int16_t ballSpeed;
    BallStateMessage(int32_t time, int16_t ballPos, int16_t ballDir, int16_t ballSpeed);
};

class NewRoundMessage: Message
{
public:
    const int32_t startTime;
    const int8_t playerScore;
    const int8_t opponentScore;
    const bool ballToPlayer;
    const int16_t ballPos;
    const int16_t ballDir;
    const int16_t ballSpeed;

    NewRoundMessage(int32_t startTime,
                    int8_t playerScore,
                    int8_t opponentScore,
                    bool ballToPlayer,
                    int16_t ballPos,
                    int16_t ballDir,
                    int16_t ballSpeed);
};

class GameEndedMessage
{
public:
    const int8_t playerScore;
    const int8_t opponentScore;

    GameEndedMessage(int8_t playerScore, int8_t opponentScore);
};

class TimeMessage
{
public:
    const int32_t time;

    explicit TimeMessage(int32_t time);
};

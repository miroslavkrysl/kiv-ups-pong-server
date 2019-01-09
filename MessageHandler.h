#pragma once

#include <unordered_map>
#include <functional>

#include "Message.h"

class Server;
class Connection;

class MessageHandler
{
    Server &server;
    std::unordered_map<std::string, std::function<void(std::unique_ptr<Message>)>> handlers;

public:
    explicit MessageHandler(Server &server);
    void handleMessage(Connection &connection, std::unique_ptr<Message> message);
};



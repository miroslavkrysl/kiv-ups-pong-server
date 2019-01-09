#include "MessageHandler.h"

MessageHandler::MessageHandler(Server &server)
    : server{server}
{
    // TODO: initialize handlers
}

void MessageHandler::handleMessage(Connection &connection, std::unique_ptr<Message> message)
{
    // TODO: implement handlers
}

#include "ConnectionWatcher.h"
#include "Server.h"

ConnectionWatcher::ConnectionWatcher(Server &server)
    : server(server)
{}

void ConnectionWatcher::run()
{
    server.getLogger().log("connection watcher running");

    // periodically check connections states and remove them eventually
    while (!shouldStop()) {

        size_t count = server.filterConnections([](Connection &connection)
                                                {
                                                    return connection.isClosed();
                                                });

        if (count) {
            server.getLogger().log(std::to_string(count) + " closed connection" + (count == 1 ? "" : "s") + " cleared");
        }

        std::this_thread::sleep_for(CHECK_PERIOD);
    }
}

bool ConnectionWatcher::stop(bool wait)
{
    return Thread::stop(wait);
}

void ConnectionWatcher::after()
{
    server.getLogger().log("connection watcher stopped", Logger::Level::Warning);
}

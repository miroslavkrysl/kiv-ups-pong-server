#include "ConnectionWatcher.h"
#include "Server.h"

ConnectionWatcher::ConnectionWatcher(Server &server)
    : server(server)
{}

void ConnectionWatcher::run()
{
    // periodically check connections states and remove them eventually
    while (!shouldStop()) {

        server.filterConnections([](Connection &connection)
                                 {
                                     return connection.isClosed();
                                 });

        server.getLogger().writeStats(server.getStats());

        std::this_thread::sleep_for(CHECK_PERIOD);
    }
}

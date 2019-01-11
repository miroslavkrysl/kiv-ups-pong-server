#include "ConnectionWatcher.h"
#include "Server.h"

ConnectionWatcher::ConnectionWatcher(Server &server)
    : server(server)
{}

void ConnectionWatcher::run()
{
    while (!shouldStop()) {

        server.filterConnections([](Connection &connection)
                                 {
                                     return connection.isClosed();
                                 });

        std::this_thread::sleep_for(CHECK_PERIOD);
    }
}

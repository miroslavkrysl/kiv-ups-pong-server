#include "ConnectionWatcher.h"
#include "Server.h"

ConnectionWatcher::ConnectionWatcher(Server &server)
    : server(server)
{}

void ConnectionWatcher::run()
{
    while (!shouldStop()) {
        std::unique_lock<std::mutex> lock{server.connectionsMutex};

        auto uidConnection = server.connections.begin();

        while (uidConnection != server.connections.end()) {
            Connection &connection = uidConnection->second;

            if (connection.isClosed()) {
                uidConnection = server.connections.erase(uidConnection);
            }
            else {
                uidConnection++;
            }
        }

        lock.release();
        std::this_thread::sleep_for(CHECK_PERIOD);
    }
}

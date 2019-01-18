#include <sstream>

#include "Stats.h"

Stats::Stats()
    : started(std::chrono::system_clock::now()),
      messagesReceived{0},
      messagesDropped{0},
      bytesReceived{0},
      bytesDropped{0},
      messagesSent{0},
      bytesSent{0}
{}

void Stats::setStarted(std::chrono::system_clock::time_point started)
{
    auto lock = acquireLock();
    Stats::started = started;
}

void Stats::addPacketsReceived(uint64_t count)
{
    auto lock = acquireLock();
    messagesReceived += count;
}

void Stats::addPacketsDropped(uint64_t count)
{
    auto lock = acquireLock();
    messagesDropped += count;
}

void Stats::addBytesReceived(uint64_t count)
{
    auto lock = acquireLock();
    bytesReceived += count;
}

void Stats::addBytesDropped(uint64_t count)
{
    auto lock = acquireLock();
    bytesDropped += count;
}

void Stats::addPacketsSent(uint64_t count)
{
    auto lock = acquireLock();
    messagesSent += count;
}

void Stats::addBytesSent(uint64_t count)
{
    auto lock = acquireLock();
    bytesSent += count;
}

std::string Stats::toLog() const
{
    auto lock = acquireLock();

    auto now = std::chrono::system_clock::now();
    std::time_t startTime = std::chrono::system_clock::to_time_t(started);
    auto upTime = now - started;

    std::stringstream stream;

    stream << "Server started at: " << std::ctime(&startTime) << std::endl;
    stream << "Already running: "
           << std::chrono::duration_cast<std::chrono::hours>(upTime).count() << " hours "
           << std::chrono::duration_cast<std::chrono::minutes>(upTime).count() << " minutes "
           << std::chrono::duration_cast<std::chrono::seconds>(upTime).count() << " seconds"
           << std::endl;
    stream << "Bytes received: " << bytesReceived + bytesDropped << std::endl;
    stream << "Packets received: " << messagesReceived + messagesDropped << std::endl;
    stream << std::endl;
    stream << "Bytes sent: " << bytesSent << std::endl;
    stream << "Packets sent: " << messagesSent << std::endl;
    stream << std::endl;
    stream << "Packets dropped: " << bytesDropped << std::endl;
    stream << "Bytes dropped: " << messagesDropped;

    return stream.str();
}

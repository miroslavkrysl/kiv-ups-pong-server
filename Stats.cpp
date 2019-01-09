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

void Stats::setStarted(const std::chrono::system_clock::time_point &started)
{
    Stats::started = started;
}

void Stats::addMessagesReceived(uint64_t count)
{
    std::unique_lock<std::mutex> lock{messagesReceivedMutex};
    messagesReceived += count;
}

void Stats::addMessagesDropped(uint64_t count)
{
    std::unique_lock<std::mutex> lock{messagesDroppedMutex};
    messagesDropped += count;
}

void Stats::addBytesReceived(uint64_t count)
{
    std::unique_lock<std::mutex> lock{bytesReceivedMutex};
    bytesReceived += count;
}

void Stats::addBytesDropped(uint64_t count)
{
    std::unique_lock<std::mutex> lock{bytesDroppedMutex};
    bytesDropped += count;
}

void Stats::addMessagesSent(uint64_t count)
{
    std::unique_lock<std::mutex> lock{messagesSentMutex};
    messagesSent += count;
}

void Stats::addBytesSent(uint64_t count)
{
    std::unique_lock<std::mutex> lock{bytesSentMutex};
    bytesSent += count;
}

std::string Stats::toLogString()
{
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
    stream << "Messages received: " << messagesReceived + messagesDropped << std::endl;
    stream << std::endl;
    stream << "Bytes sent: " << bytesSent << std::endl;
    stream << "Messages sent: " << messagesSent << std::endl;
    stream << std::endl;
    stream << "Messages dropped: " << bytesDropped << std::endl;
    stream << "Bytes dropped: " << messagesDropped << std::endl;

    return stream.str();
}

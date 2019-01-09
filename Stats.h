#pragma once

#include <string>
#include <chrono>
#include <mutex>

class Stats
{
    std::chrono::system_clock::time_point started;
    uint64_t messagesReceived;
    uint64_t messagesDropped;
    uint64_t bytesReceived;
    uint64_t bytesDropped;
    uint64_t messagesSent;
    uint64_t bytesSent;

    std::mutex messagesReceivedMutex;
    std::mutex messagesDroppedMutex;
    std::mutex bytesReceivedMutex;
    std::mutex bytesDroppedMutex;
    std::mutex messagesSentMutex;
    std::mutex bytesSentMutex;

public:
    Stats();

    void setStarted(const std::chrono::system_clock::time_point &started);
    void addMessagesReceived(uint64_t count);
    void addMessagesDropped(uint64_t count);
    void addBytesReceived(uint64_t count);
    void addBytesDropped(uint64_t count);
    void addMessagesSent(uint64_t count);
    void addBytesSent(uint64_t count);

    std::string toLogString();
};



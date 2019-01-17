#pragma once

#include <string>
#include <fstream>

#include "../Util/Stats.h"
#include "../Network/Packet.h"
#include "../Network/Connection.h"

class Logger
{
public:
    enum class Level
    {
        Default,
        Success,
        Warning,
        Error
    };

private:
    std::string baseLogFileName;
    std::string communicationLogFileName;
    std::string statsFileName;

    std::mutex baseLogMutex;
    std::mutex communicationLogMutex;
    std::mutex statsMutex;

    std::fstream baseLogFile;
    std::fstream communicationLogFile;
    std::fstream statsFile;

public:
    Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile);
    virtual ~Logger();

    void log(const std::string &message, Level level = Level::Default);
    void logCommunication(const Packet &packet, bool incoming, Uid uid);
    void writeStats(const Stats &stats);
};

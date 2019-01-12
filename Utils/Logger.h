#pragma once

#include <string>
#include <fstream>
#include "Stats.h"
#include "../Network/Packet.h"

class Logger
{
public:
    enum class Level{
        Default,
        Success,
        Warning,
        Error
    };

private:
    std::fstream baseLogFile;
    std::fstream communicationLogFile;
    std::fstream statsFile;

public:
    Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile);
    virtual ~Logger();

    void log(const std::string &message, Level level = Level::Default);
    void logCommunication(Packet &packet, bool incoming, std::string id = "");
    void writeStats(Stats &stats);
};

class LoggerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};
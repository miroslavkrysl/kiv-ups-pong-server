#pragma once

#include <string>
#include <fstream>
#include "Stats.h"
#include "Packet.h"

class Logger
{
public:
    enum class Level{
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

    void log(std::string &message, Level level);
    void logCommunication(Packet &packet, bool incoming);
    void writeStats(Stats &stats);
};

class LoggerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};
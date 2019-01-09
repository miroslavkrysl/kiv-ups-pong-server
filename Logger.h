#pragma once

#include <string>
#include <fstream>
#include "Message.h"
#include "Stats.h"

class Logger
{
    std::fstream baseLogFile;
    std::fstream communicationLogFile;
    std::fstream statsFile;
public:
    Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile);
    virtual ~Logger();
    void log(std::string &message);
    void logCommunication(Message &message);
    void writeStats(Stats &stats);
};

class LoggerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};
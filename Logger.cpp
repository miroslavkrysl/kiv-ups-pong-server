#include "Logger.h"

Logger::Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile)
{
    baseLogFile_.open(baseLogFile, std::ios::out);
    if (!baseLogFile_.is_open()) {
        throw LoggerException("can not open file " + baseLogFile + " for writing");
    }

    communicationLogFile_.open(communicationLogFile, std::ios::out);
    if (!communicationLogFile_.is_open()) {
        throw LoggerException("can not open file " + communicationLogFile + " for writing");
    }

    statsFile_.open(statsFile, std::ios::out);
    if (!statsFile_.is_open()) {
        throw LoggerException("can not open file " + statsFile + " for writing");
    }
}

void Logger::log(std::string &message)
{
    baseLogFile_ << message << std::endl;
}

void Logger::logCommunication(Message &message)
{
    communicationLogFile_ << message.toLogString() << std::endl;
}

void Logger::writeStats(Stats &stats)
{
    statsFile_ << stats.toLogString() << std::endl;
}

Logger::~Logger()
{
    baseLogFile_.close();
    communicationLogFile_.close();
    statsFile_.close();
}

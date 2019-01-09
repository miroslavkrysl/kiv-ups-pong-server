#include "Logger.h"

Logger::Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile)
{
    this->baseLogFile.open(baseLogFile, std::ios::out);
    if (!this->baseLogFile.is_open()) {
        throw LoggerException("can not open file " + baseLogFile + " for writing");
    }

    this->communicationLogFile.open(communicationLogFile, std::ios::out);
    if (!this->communicationLogFile.is_open()) {
        throw LoggerException("can not open file " + communicationLogFile + " for writing");
    }

    this->statsFile.open(statsFile, std::ios::out);
    if (!this->statsFile.is_open()) {
        throw LoggerException("can not open file " + statsFile + " for writing");
    }
}

void Logger::log(std::string &message)
{
    baseLogFile << message << std::endl;
}

void Logger::logCommunication(Message &message)
{
    communicationLogFile << message.toLogString() << std::endl;
}

void Logger::writeStats(Stats &stats)
{
    statsFile << stats.toLogString() << std::endl;
}

Logger::~Logger()
{
    baseLogFile.close();
    communicationLogFile.close();
    statsFile.close();
}

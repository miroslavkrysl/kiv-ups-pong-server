#include <iostream>
#include <sstream>

#include "Logger.h"
#include "Decor.h"

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

void Logger::log(const std::string &message, Level level)
{
    Decor color;
    Decor reset;

    switch (level) {
    case Level::Default: color = Decor{Decor::FG_DEFAULT};
        break;
    case Level::Success: color = Decor{Decor::FG_GREEN};
        break;
    case Level::Warning: color = Decor{Decor::FG_YELLOW};
        break;
    case Level::Error: color = Decor{Decor::FG_RED};
        break;
    }

    std::cout << color << message << reset << std::endl;
    baseLogFile << message << std::endl;
}

void Logger::logCommunication(Packet &packet, bool incoming, std::string id)
{
    std::string serialized;

    if (incoming) {
        std::cout << Decor{Decor::FG_GREEN} << "<- " << id << ": " << serialized << Decor{Decor::FG_DEFAULT}
                  << std::endl;
    }
    else {
        std::cout << Decor{Decor::FG_MAGENTA} << "-> " << id << ": " << serialized << Decor{Decor::FG_DEFAULT}
                  << std::endl;
    }

    communicationLogFile << serialized << std::endl;
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

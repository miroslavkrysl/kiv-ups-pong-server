#include <iostream>
#include <sstream>

#include "Logger.h"
#include "Decor.h"

Logger::Logger(std::string baseLogFile, std::string communicationLogFile, std::string statsFile)
    : baseLogFileName(baseLogFile),
      communicationLogFileName(communicationLogFile),
      statsFileName(statsFile)
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

    std::lock_guard<std::mutex> lock{baseLogMutex};

    std::cout << color << message << reset << std::endl;
    baseLogFile << message << std::endl;
}

void Logger::logCommunication(Packet &packet, bool incoming, std::string id)
{
    std::ostringstream serialized;
    std::string serializedPacket = packet.toLogString();

    if (incoming) {
        serialized << Decor{Decor::FG_BLUE} << "<- " << id << ": " << serializedPacket << Decor{Decor::FG_DEFAULT};
    }
    else {
        serialized << Decor{Decor::FG_MAGENTA} << "-> " << id << ": " << serializedPacket << Decor{Decor::FG_DEFAULT};
    }

    std::lock_guard<std::mutex> lock{communicationLogMutex};
    std::cout << serialized.str() << std::endl;
    communicationLogFile << serialized.str() << std::endl;
}

void Logger::writeStats(Stats &stats)
{
    std::lock_guard<std::mutex> lock{statsMutex};
    statsFile.close();
    statsFile.open(statsFileName);
    statsFile << stats.toLogString() << std::endl;
}

Logger::~Logger()
{
    baseLogFile.close();
    communicationLogFile.close();
    statsFile.close();
}

#include <iostream>
#include <sstream>

#include "Logger.h"
#include "Text.h"
#include "../Network/Connection.h"

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
    std::string color;

    switch (level) {
    case Level::Default: {
        color = Text::decor(Text::FG_DEFAULT);
        break;
    }
    case Level::Success: {
        color = Text::decor(Text::FG_GREEN);
        break;
    }
    case Level::Warning: {
        color = Text::decor(Text::FG_YELLOW);
        break;
    }
    case Level::Error: {
        color = Text::decor(Text::FG_RED);
        break;
    }
    }

    std::lock_guard<std::mutex> lock{baseLogMutex};

    std::cout << color << message << Text::decor() << std::endl;
    baseLogFile << message << std::endl;
}

void Logger::logCommunication(Packet &packet, bool incoming, Uid id)
{
    std::string color;
    std::string arrow;

    if (incoming) {
        color = Text::decor(Text::FG_BLUE);
        arrow = "<-";
    }
    else {
        color = Text::decor(Text::FG_MAGENTA);
        arrow = "->";
    }

    std::string serialized = packet.toLogString();

    communicationLogMutex.lock();

    std::cout << color << arrow << " " << id << ": " << serialized << std::endl;
    communicationLogFile << serialized << std::endl;

    communicationLogMutex.unlock();
}

void Logger::writeStats(Stats &stats)
{
    statsMutex.lock();

    statsFile.close();
    statsFile.open(statsFileName);
    statsFile << stats.toLogString() << std::endl;

    statsMutex.unlock();
}

Logger::~Logger()
{
    baseLogFile.close();
    communicationLogFile.close();
    statsFile.close();
}

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

#include "Thread.h"

class App;

class Shell: public Thread
{
    std::istream &input;
    std::ostream &output;
    App &app;

    typedef void (Shell::*Command)(std::vector<std::string>);

    std::unordered_map<std::string, Command> commands{
        {"exit", &Shell::cmdExit},
        {"games", &Shell::cmdGames},
        {"players", &Shell::cmdPlayers},
        {"info", &Shell::cmdInfo},
        {"stats", &Shell::cmdStats},
        {"help", &Shell::cmdHelp},
    };

    void handle(std::string command);

    void cmdHelp(std::vector<std::string> arguments);
    void cmdInfo(std::vector<std::string> arguments);
    void cmdStats(std::vector<std::string> arguments);
    void cmdPlayers(std::vector<std::string> arguments);
    void cmdGames(std::vector<std::string> arguments);
    void cmdExit(std::vector<std::string> arguments);

public:
    explicit Shell(App &app);

    void run() override;
    void after() override;
};

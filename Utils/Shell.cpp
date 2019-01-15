#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

#include "Shell.h"
#include "Text.h"
#include "../Network/Server.h"

Shell::Shell(Server &server)
    : input(std::cin),
      output(std::cout),
      server(server)
{}

void Shell::run()
{
    server.getLogger().log("shell running");

    while (!shouldStop() && !input.eof()) {
        std::string command;
        getline(input, command);

        if (command.empty()) {
            continue;
        }

        try {
            handle(command);
        }
        catch (std::exception &exception) {
            output << "error: " << exception.what() << std::endl;
        }
    }
}

void Shell::after()
{
    server.stop(false);
    server.getLogger().log("shell stopped", Logger::Level::Warning);
}

void Shell::handle(std::string line)
{
    // split command into arguments
    std::stringstream lineStream{line};
    std::vector<std::string> arguments{
        std::istream_iterator<std::string>{lineStream},
        std::istream_iterator<std::string>{}};

    if (arguments.empty()) {
        return;
    }

    std::string commandName{arguments[0]};

    auto found = Shell::commands.find(commandName);

    if (found == Shell::commands.end()) {
        throw UnknownCommandException("unknown command");
    }

    Command command = found->second;

    (this->*command)(arguments);
}

void Shell::cmdHelp(std::vector<std::string> arguments)
{
    output << std::endl;
    output << Text::hline() << std::endl;

    output << "Help:" << std::endl << std::endl;

    output << Text::justifyL("help", 10) << "- print help" << std::endl;
    output << Text::justifyL("info", 10) << "- print server info" << std::endl;
    output << Text::justifyL("stats", 10) << "- print server statistics" << std::endl;
    output << Text::justifyL("players", 10) << "- print list of players" << std::endl;
    output << Text::justifyL("games", 10) << "- print list of games" << std::endl;
    output << Text::justifyL("exit", 10) << "- stop the server and exit" << std::endl;

    output << Text::hline() << std::endl;
    output << std::endl;
}

void Shell::cmdInfo(std::vector<std::string> arguments)
{
    output << std::endl;
    output << Text::hline() << std::endl;
    output << "Info:" << std::endl << std::endl;
    output << server.toLogString() << std::endl;
    output << Text::hline() << std::endl;
    output << std::endl;
}

void Shell::cmdStats(std::vector<std::string> arguments)
{
    output << std::endl;
    output << Text::hline() << std::endl;
    output << "Statistics:" << std::endl << std::endl;
    output << server.getStats().toLogString() << std::endl;
    output << Text::hline() << std::endl;
    output << std::endl;
}

void Shell::cmdPlayers(std::vector<std::string> arguments)
{
    auto &output = this->output;

    output << std::endl;
    output << Text::hline() << std::endl;

    output << "Players:" << std::endl << std::endl;

    size_t count = server.forEachConnection([&output](Connection &connection){
        output << "- " << connection.getUid() << std::endl;
    });

    if (count == 0) {
        output << "- no players active -" << std::endl;
    }

    output << Text::hline() << std::endl;
    output << std::endl;
}

void Shell::cmdGames(std::vector<std::string> arguments)
{
    auto &output = this->output;

    std::vector<std::string> pending;

    output << std::endl;
    output << Text::hline() << std::endl;

    output << "Private games:" << std::endl << std::endl;

    size_t countPrivate = server.forEachPrivateGame([&output, &pending](Game &game){
        auto nicknames = game.getNicknames();

        if (game.isNew()) {
            pending.insert(nicknames.first);
            return;
        }

        output << "- "
               << Text::justifyR(nicknames.first, 16) << " vs. " << Text::justifyR(nicknames.second, 16) << std::endl;
    });

    output << "Public games:" << std::endl << std::endl;

    size_t countPublic = server.forEachPublicGame([&output, &pending](Game &game){
        auto nicknames = game.getNicknames();

        if (game.isNew()) {
            pending.insert(nicknames.first);
            return;
        }

        output << "- "
            << Text::justifyR(nicknames.first, 16) << " vs. " << Text::justifyR(nicknames.second, 16) << std::endl;
    });

    output << "Pending:" << std::endl << std::endl;

    for (auto &nickname : pending) {
        output << "- " << nickname << std::endl;
    }

    if (countPrivate + countPublic == 0) {
        output << "- no games active -" << std::endl;
    }

    output << Text::hline() << std::endl;
    output << std::endl;
}

void Shell::cmdExit(std::vector<std::string> arguments)
{
    stop(false);
    server.stop(false);
}

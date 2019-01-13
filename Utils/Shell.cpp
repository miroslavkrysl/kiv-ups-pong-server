#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>

#include "Shell.h"
#include "../Network/Server.h"

Shell::Shell(const std::istream &input, const std::ostream &output, Server &server)
    : input(const_cast<std::istream &>(input)),
      output(const_cast<std::ostream &>(output)),
      server(server)
{}

void Shell::run()
{
    server.getLogger().log("shell running");

    while (!shouldStop() && !input.eof()) {
        std::string command;
        getline(input, command);

        if (input.eof()) {
            break;
        }

        try {
            handle(command);
        }
        catch (std::exception &exception) {
            output << "error: " << exception.what() << std::endl;
        }
    }
}

void Shell::handle(std::string line)
{
    std::stringstream lineStream{line};
    std::vector<std::string> arguments{
        std::istream_iterator<std::string>{lineStream},
        std::istream_iterator<std::string>{}};

    if (arguments.empty()) {
        return;
    }

    std::string commandName{arguments[0]};

    if (Shell::actions.find(commandName) == Shell::actions.end()) {
        throw UnknownCommandException("unknown command");
    }

    Command command = Shell::actions[commandName];

    (this->*command)(arguments);
}

void Shell::cmdExit(std::vector<std::string> arguments)
{
    stop(false);
    server.stop(false);
}

void Shell::cmdGames(std::vector<std::string> arguments)
{
    auto &output = this->output;

    size_t count = server.forEachConnection([&output](Connection &connection){
        // TODO: print games
        output << "game" << std::endl;
    });

    if (count == 0) {
        output << "-no games active-" << std::endl;
    }
}

void Shell::cmdPlayers(std::vector<std::string> arguments)
{
    auto &output = this->output;

    size_t count = server.forEachConnection([&output](Connection &connection){
        // TODO: print players
        output << "player" << std::endl;
    });

    if (count == 0) {
        output << "-no players active-" << std::endl;
    }
}

void Shell::cmdInfo(std::vector<std::string> arguments)
{
    output << server.getStats().toLogString();
}

void Shell::cmdHelp(std::vector<std::string> arguments)
{
    // TODO: print help
    output << "info" << std::endl;
}

void Shell::after()
{
    server.getLogger().log("shell stopped", Logger::Level::Warning);
}

bool Shell::stop(bool wait)
{
    input.setstate(std::istream::failbit);
    return Thread::stop(wait);
}

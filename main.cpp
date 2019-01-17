#include <iostream>
#include <getopt.h>
#include <csignal>
#include "App.h"

App *app = nullptr;

void signalHandler(int signum) {
    if (app) {
        app->stop(false);
    }
}

bool isValidPort(unsigned long port)
{
    return port > 0 && port <= UINT16_MAX;
}

void printHelp(char *name)
{
    std::cout << "Pong game server with a simple built-in shell." << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << name << " [-t port] [-i ip_address]" << std::endl;
    std::cout << std::endl;
    std::cout << "\t-t port" << std::endl;
    std::cout << "\t\tdefault = 8191" << std::endl;
    std::cout << "\t\tPort number in range 1 - 65535." << std::endl;
    std::cout << std::endl;
    std::cout << "\t-i ip_address" << std::endl;
    std::cout << "\t\tdefault = 0.0.0.0" << std::endl;
    std::cout << "\t\tIPv4 address in the Internet standard dot notation." << std::endl;
    std::cout << std::endl;
    std::cout << "\t-h";
    std::cout << "\t\tPrint help";
}

int main(int argc, char *argv[])
{
    Port port = App::DEFAULT_PORT;
    std::string ip;

    int opt;

    while ((opt = getopt(argc, argv, "hp:i:")) != -1) {
        switch (opt) {
        case 'p': {
            unsigned long p = std::stoul(std::string{optarg});
            if (!isValidPort(p)) {
                std::cout << "error: invalid port number" << std::endl;
                exit(EXIT_FAILURE);
            }
            port = static_cast<uint16_t>(p);
            break;
        }
        case 'i': {
            ip = optarg;
            break;
        }
        case 'h': {
            printHelp(argv[0]);
            exit(EXIT_SUCCESS);
        }
        default:{
            exit(EXIT_FAILURE);
        }
        }
    }

    try {
        app = new App{port, ip};
        app->start();

        // register sigterm and sigint handler
        signal(SIGTERM, signalHandler);
        signal(SIGINT, signalHandler);

        app->join();
        delete app;
    }
    catch (std::exception &exception) {
        std::cout << "Exception: " << exception.what();
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
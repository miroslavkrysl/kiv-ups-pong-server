#include <iostream>
#include <getopt.h>

#include "Network/Server.h"
#include "Utils/Shell.h"

bool isValidPort(unsigned long port)
{
    return port > 0 && port <= UINT16_MAX;
}

void printHelp(char *name)
{
    printf("Pong game server with a simple built-in shell.\n");
    printf("\n");
    printf("Usage:\n");
    printf("%s [-t port] [-i ip_address]\n", name);
    printf("\n");
    printf("\t-t port\n");
    printf("\t\tdefault = 8191\n");
    printf("\t\tPort number in range 1 - 65535.\n");
    printf("\n");
    printf("\t-i ip_address\n");
    printf("\t\tdefault = 0.0.0.0\n");
    printf("\t\tIPv4 adrres in the Internet standard dot notation.\n");
    printf("\n");
    printf("\t-h\n");
    printf("\t\tPrint help\n");
}

int main(int argc, char *argv[])
{
    uint16_t port = 8191;
    std::string ip;

    int opt;

    while ((opt = getopt(argc, argv, "hp:i:")) != -1) {
        switch (opt) {
        case 'p': {
            unsigned long p = std::stoul(std::string{optarg});
            if (!isValidPort(p)) {
                printf("error: invalid port number");
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
        default: {
            printf("error: invalid option %c\n\n", opt);
            printHelp(argv[0]);
            exit(EXIT_FAILURE);
        }
        }
    }

    try {
        Server server{port, ip};
        server.start();
        server.join();
    }
    catch (std::exception &exception) {
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
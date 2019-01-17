#pragma once

#include <netinet/in.h>

#include <string>

#include "../Types.h"
#include "../Util/Thread.h"

class App;

class Server: public Thread
{
    App &app;

    sockaddr_in address;
    std::string ip;
    Port port;
    int socket;

public:
    explicit Server(App &app, Port port, std::string ip = "");

    const sockaddr_in &getAddress();
    std::string getIp();
    Port getPort();

    void before() override;
    void run() override;
    bool stop(bool wait) override;
    void after() override;

    std::string toLogString();
};


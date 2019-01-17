#pragma once

#include "../Util/Thread.h"
#include "../Types.h"

class App;

class Game: public Thread
{
    App &app;
    Uid uid;
public:
    Game(App &app, Uid uid);
    void run() override;
};



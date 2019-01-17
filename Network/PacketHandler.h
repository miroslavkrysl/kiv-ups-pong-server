#pragma once

#include "Packet.h"
#include "../Types.h"

class App;

class PacketHandler
{
    App &app;

public:
    explicit PacketHandler(App &app);
    void handleIncoming(Uid uid, const Packet &packet);
    void handleOutgoing(Uid uid, const Packet &packet);
};



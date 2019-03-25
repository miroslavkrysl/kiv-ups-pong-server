#pragma once

#include <chrono>
#include <netinet/in.h>

class Connection {
public:
    inline static const std::chrono::seconds INACTIVE_TIMEOUT{10};
    inline static const std::chrono::seconds RECV_TIMEOUT{1};
    inline static const std::chrono::seconds SEND_TIMEOUT{1};

    Connection(PacketFactory &packetFactory, sockaddr_in address);

    IPacket receivePacket();
    sendPacket(IPacket Packet);
private:
    PacketHandler &packetHandler;
    PacketFactory &packetFactory;
    uint16_t port;
    std::string ip;
    int socket;
    sockaddr_in address;
};

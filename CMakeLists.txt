cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp
        App.cpp App.h
        Exceptions.h
        Util/Logger.cpp Util/Logger.h
        Util/Text.cpp Util/Text.h
        Util/Stats.cpp Util/Stats.h
        Types.cpp Types.h
        Util/Thread.cpp Util/Thread.h
        Networ/Server.cpp Networ/Server.h
        Networ/Connection.cpp Networ/Connection.h
        Networ/Packet.cpp Networ/Packet.h
        Networ/PacketHandler.cpp Networ/PacketHandler.h
        )

TARGET_LINK_LIBRARIES(ups pthread)

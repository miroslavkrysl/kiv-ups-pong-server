cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp
        App.cpp App.h
        Exceptions.h
        Types.cpp Types.h

        Utils/Logger.cpp Utils/Logger.h
        Utils/Shell.cpp Utils/Shell.h
        Utils/Text.cpp Utils/Text.h
        Utils/Stats.cpp Utils/Stats.h
        Utils/Thread.cpp Utils/Thread.h
        Utils/Lockable.cpp Utils/Lockable.h

        Network/Server.cpp Network/Server.h
        Network/Connection.cpp Network/Connection.h
        Network/Packet.cpp Network/Packet.h
        Network/PacketHandler.cpp Network/PacketHandler.h

        Game/Game.cpp Game/Game.h
        Game/GameTypes.cpp Game/GameTypes.h
        Game/BallState.cpp Game/BallState.h
        Game/PlayerState.cpp Game/PlayerState.h)

TARGET_LINK_LIBRARIES(ups pthread)

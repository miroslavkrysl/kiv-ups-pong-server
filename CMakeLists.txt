cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp
        App.cpp App.h
        Exceptions.h
        Types.cpp Types.h

        Util/Logger.cpp Util/Logger.h
        Util/Text.cpp Util/Text.h
        Util/Stats.cpp Util/Stats.h
        Util/Thread.cpp Util/Thread.h
        Util/Lockable.cpp Util/Lockable.h

        Network/Server.cpp Network/Server.h
        Network/Connection.cpp Network/Connection.h
        Network/Packet.cpp Network/Packet.h
        Network/PacketHandler.cpp Network/PacketHandler.h

        Game/Game.cpp Game/Game.h
        Game/GameTypes.cpp Game/GameTypes.h
        Game/BallState.cpp Game/BallState.h
        Game/PlayerState.cpp Game/PlayerState.h)

TARGET_LINK_LIBRARIES(ups pthread)

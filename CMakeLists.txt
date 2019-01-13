cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp Network/Server.cpp Network/Server.h Network/Connection.cpp Network/Connection.h Utils/Thread.cpp Utils/Thread.h Utils/Logger.cpp Utils/Logger.h Utils/Stats.cpp Utils/Stats.h Game/PlayerState.cpp Game/PlayerState.h Network/Packet.cpp Network/Packet.h Network/ConnectionAcceptor.cpp Network/ConnectionAcceptor.h Network/ConnectionWatcher.cpp Network/ConnectionWatcher.h Utils/Shell.cpp Utils/Shell.h Utils/Decor.cpp Utils/Decor.h Game/Game.cpp Game/Game.h Utils/Serializable.h Game/BallState.cpp Game/BallState.h Utils/StateMachine.cpp Utils/StateMachine.h Game/GameStateMachine.cpp Game/GameStateMachine.h Game/GameTypes.h Game/GameTypes.cpp)

TARGET_LINK_LIBRARIES(ups pthread)

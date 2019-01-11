cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp Server.cpp Server.h Connection.cpp Connection.h Thread.cpp Thread.h Logger.cpp Logger.h Stats.cpp Stats.h Player.cpp Player.h Packet.cpp Packet.h ConnectionAcceptor.cpp ConnectionAcceptor.h ConnectionWatcher.cpp ConnectionWatcher.h Shell.cpp Shell.h)

TARGET_LINK_LIBRARIES(ups pthread)

cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups main.cpp Server.cpp Server.h Connection.cpp Connection.h StateMachine.cpp StateMachine.h)

TARGET_LINK_LIBRARIES(ups pthread)

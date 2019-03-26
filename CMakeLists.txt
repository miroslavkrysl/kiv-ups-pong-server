cmake_minimum_required(VERSION 3.13)
project(kiv-ups-pongserver)

set(CMAKE_CXX_STANDARD 17)
find_package (Threads REQUIRED)

add_executable(kiv-ups-pongserver src/main.cpp
        src/Network/Socket.cpp src/Network/Socket.h
        )

TARGET_LINK_LIBRARIES(kiv-ups-pongserver pthread)

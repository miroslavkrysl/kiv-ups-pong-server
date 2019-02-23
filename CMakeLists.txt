cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads REQUIRED)

add_executable(ups src/main.cpp
        src/App.cpp src/App.h
        src/AppException.cpp src/AppException.h)

TARGET_LINK_LIBRARIES(ups pthread)

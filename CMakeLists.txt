cmake_minimum_required(VERSION 3.13)
project(ups)

set(CMAKE_CXX_STANDARD 14)

add_executable(ups main.cpp Server.cpp Server.h)
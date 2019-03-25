cmake_minimum_required(VERSION 3.13)
project(kiv-ups-pongserver)

set(CMAKE_CXX_STANDARD 17)
find_package (Threads REQUIRED)

add_executable(kiv-ups-pongserver src/main.cpp
        src/App.cpp src/App.h
        src/EApp.cpp src/EApp.h
        src/User.cpp src/User.h

        src/Network/Session.cpp src/Network/Session.h
        src/Network/Connection.cpp src/Network/Connection.h

        src/Utils/Thread.cpp src/Utils/Thread.h
        src/Utils/TokenGenerator.cpp src/Utils/TokenGenerator.h
        src/Utils/Monitor.cpp src/Utils/Monitor.h src/Network/TcpStream.cpp src/Network/TcpStream.h src/Network/Exceptions/EInvalidSocket.cpp src/Network/Exceptions/EInvalidSocket.h src/Network/Exceptions/ETcpStreamIrecoverable.cpp src/Network/Exceptions/ETcpStreamIrecoverable.h src/temp/Network/Port.cpp src/temp/Network/Port.h src/Network/Socket.cpp src/Network/Socket.h)

TARGET_LINK_LIBRARIES(kiv-ups-pongserver pthread)

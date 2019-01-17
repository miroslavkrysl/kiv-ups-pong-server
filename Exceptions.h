#pragma once

#include <stdexcept>

// app

class AppException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class NotLoggedException: public AppException
{
    using AppException::AppException;
};

class AlreadyLoggedException: public AppException
{
    using AppException::AppException;
};

// logger

class LoggerException: public AppException
{
    using AppException::AppException;
};

// server

class ServerException: public AppException
{
    using AppException::AppException;
};

// connection

class ConnectionException: public AppException
{
    using AppException::AppException;
};

// packets

class PacketException: public AppException
{
    using AppException::AppException;
};

class MalformedPacketException: public PacketException
{
    using PacketException::PacketException;
};

class NonContextualPacketException: public PacketException
{
    using PacketException::PacketException;
};

// game

class GameException: public AppException
{
    using AppException::AppException;
};

class GameTypeException: public GameException
{
    using GameException::GameException;
};

class GameEventException: public GameException
{
    using GameException::GameException;
};

class GamePlayException: public GameException
{
    using GameException::GameException;
};

//class UnknownPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
//

//
//class InvalidPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
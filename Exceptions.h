#pragma once

#include <stdexcept>

// logger

class LoggerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// server

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// connection

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

// packets

class PacketException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class MalformedPacketException: public PacketException
{
    using PacketException::PacketException;
};

//class UnknownPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
//
//class NonContextualPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};

//
//class InvalidPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
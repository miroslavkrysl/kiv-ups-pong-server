#pragma once

#include <stdexcept>

class LoggerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class ServerException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

class ConnectionException: public std::runtime_error
{
    using std::runtime_error::runtime_error;
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
//class MalformedPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
//
//class InvalidPacketException: public PacketException
//{
//    using PacketException::PacketException;
//};
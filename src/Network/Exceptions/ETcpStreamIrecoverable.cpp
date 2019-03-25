#include "ETcpStreamIrecoverable.h"

const char *ETcpStreamIrecoverable::what() const noexcept
{
    std::string message{"A tcp stream error occurred"};
    return message.c_str();
}

#include <utility>
#include <stdexcept>
#include <sstream>

#include "Exception.h"

namespace pong
{

Exception::Exception(std::string description)
    : description(std::move(description))
{}

const char *Exception::what() const noexcept
{
    return description.c_str();
}

std::string Exception::whatNested() const
{
    std::stringstream message;
    message << what();

    const std::exception *current{this};
    size_t depth = 1;

    while (true)
    {
        try
        {
            std::rethrow_if_nested(*current);
            break;
        }
        catch (std::exception &e)
        {
            message << std::endl;
            message << std::string(depth * 3, ' ');
            message << "|- " << e.what();

            current = &e;
            depth++;
        }
    }

    return message.str();
}

}

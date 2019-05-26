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
    int depth = 1;

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

            for (int i = 0; i < depth; ++i)
            {
                message << "   ";
            }

            message << "|- " << e.what();

            current = &e;
            depth++;
        }
    }

    return message.str();
}

}

#include <sstream>
#include <iomanip>

#include "Text.h"

namespace pong::utils
{

std::string Text::hLine(uint16_t width, char lineChar)
{
    return std::string(width, lineChar);
}

std::string Text::align(const std::string &string, uint16_t width, Align align, char fillChar)
{
    std::stringstream ss;


    switch (align)
    {
        case Align::LEFT:
            ss << std::setw(width);
            ss << std::left;
            break;
        case Align::CENTER:
            ss << std::setw(width / 2 + string.length() / 2);
            ss << std::right;
            break;
        case Align::RIGHT:
            ss << std::setw(width);
            ss << std::right;
            break;
    }

    ss << std::setfill(fillChar);
    ss << string;

    return ss.str();
}

}
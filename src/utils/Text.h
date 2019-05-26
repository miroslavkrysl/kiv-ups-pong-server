#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace pong::utils
{

/**
 * Utility class for text formatting.
 */
class Text
{
public:
    /**
     * Enum for text aligning.
     */
    enum class Align
    {
        LEFT,
        CENTER,
        RIGHT
    };

    /**
     * Fill char for creating horizontal line.
     */
    static const char HORIZONTAL_LINE_CHAR{'-'};

    /**
     * Create horizontal line of given width.
     *
     * @param width The width of the line.
     * @param lineChar The char to fill the line.
     * @return The horizontal line.
     */
    static std::string hLine(uint16_t width = 50, char lineChar = HORIZONTAL_LINE_CHAR);

    /**
     * Align text.
     * Remaining space will be filled with the given fill char,
     *
     * @param string The string to align.
     * @param width The width to align.
     * @param align The align way.
     * @param fillChar The fill char.
     * @return The aligned text.
     */
    static std::string align(const std::string &string, uint16_t width, Align align, char fillChar = ' ');
};

}

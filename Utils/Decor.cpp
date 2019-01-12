#include <iostream>

#include "Decor.h"

Decor::Decor()
    :code(RESET_ALL)
{}

Decor::Decor(int8_t code)
    : code(code)
{}

std::ostream &operator<<(std::ostream &out, const Decor &text)
{
    out << "\33[" + std::to_string(static_cast<int>(text.code)) + "m";
    return out;
}

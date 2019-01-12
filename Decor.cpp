#include "Decor.h"
#include <iostream>

Decor::Decor()
    :code(RESET_ALL)
{}

Decor::Decor(int8_t code)
    : code(code)
{}

std::ostream &operator<<(std::ostream &out, const Decor &text)
{
    out << "\033[" << text.code << "m";
    return out;
}

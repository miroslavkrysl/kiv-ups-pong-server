#include "Exception.h"

const char *Exception::what() const noexcept
{
    std::string message;

    message += "Error: " + description;

    if (!solution.empty()) {
        message += "Context: " + context;
    }

    if (!solution.empty()) {
        message += "Solution: " + solution;
    }

    return message.c_str();
}

Exception::Exception(const std::string &description, const std::string &context, const std::string &solution)
    : description(description), context(context), solution(solution)
{}

#include <chrono>
#include <algorithm>

#include "TokenGenerator.h"

TokenGenerator::TokenGenerator()
    : mRandomEngine{static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count())}
{}

std::string TokenGenerator::GetRandomToken(std::size_t length, const std::string &characters)
{
    std::uniform_int_distribution<size_t> distribution{0, characters.size() - 1};

    std::string token;
    std::generate_n(std::back_inserter(token), length, [&] {
        return characters[distribution(mRandomEngine)];
    });

    return token;
}

std::string TokenGenerator::GetRandomToken(std::size_t length)
{
    return GetRandomToken(length, mDefaultTokenCharacters);
}
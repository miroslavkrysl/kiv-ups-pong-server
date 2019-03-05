#include <chrono>
#include <algorithm>

#include "TokenGenerator.h"

TokenGenerator::TokenGenerator()
    : mRandomEngine{static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count())}
{}

std::string TokenGenerator::GetRandomToken(std::size_t length , )
{
    std::uniform_int_distribution<size_t> distribution{0, mDefaultTokenCharacters.size() - 1};

    std::string token;
    std::generate_n(std::back_inserter(token), length, [&] {
        return mDefaultTokenCharacters[distribution(mRandomEngine)];
    });

    return token;
}
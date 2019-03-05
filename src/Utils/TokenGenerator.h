#pragma once

#include <string>
#include <random>

class TokenGenerator
{
public:
    /**
     * Initialize a new TokenGenerator.
     * The current epoch time will be used as a random engine seed.
     */
    TokenGenerator();

    /**
     * Generate a random token.
     *
     * @param length The length of the resulting token.
     * @return A random token.
     */
    std::string GetRandomToken(std::size_t length);

private:
    /**
     * Random engine for generating random values.
     */
    std::default_random_engine mRandomEngine;

    /**
     * The default set of characters available for the tokens generation.
     */
    const std::string mDefaultTokenCharacters{"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"};

    /**
     * The default token length.
     */
    const size_t mDefaultTokenLength{64};
};

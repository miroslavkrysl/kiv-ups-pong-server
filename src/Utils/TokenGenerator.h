#pragma once

#include <string>
#include <random>

/**
 * The class TokenGenerator contains a logic for the tokens generation.
 */
class TokenGenerator
{
public:
    /**
     * Initialize a new TokenGenerator.
     * The current epoch time will be used as a random engine seed.
     */
    TokenGenerator();

    /**
     * Generate a random token from the given characters.
     *
     * @param length The length of the resulting token.
     * @param characters The set of the characters which will be used for the token generation.
     *
     * @return A random token.
     */
    std::string GetRandomToken(std::size_t length, const std::string &characters);

    /**
     * Generate a random token.
     * The default set of characters will be used for the token generation.
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

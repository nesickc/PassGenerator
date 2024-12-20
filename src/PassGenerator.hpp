#pragma once
#include <string>
#include <vector>

class PassGenerator
{
public:
    PassGenerator(std::string seed);
    std::string GeneratePassword( std::string targetName );

private:
    std::string m_seed;
    std::vector<unsigned char> m_key;

    inline static const std::string m_salt = "ofQ08azDkAOE6FXl";
    inline static const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=";
    inline static const int KEY_SIZE = 32;
    inline static const int PASSWORD_SIZE = 16;
};


#pragma once
#include <string>
#include <vector>

#include "PlatformManager.hpp"

class PassGenerator
{
public:
    PassGenerator(std::string seed);
    std::string GeneratePassword( std::string targetName );
    const PlatformManager& GetPlatformManager() { return m_platformManager; }

private:
    std::vector<unsigned char> GenerateKey( std::string seed );

private:
    std::vector<unsigned char> m_key;
    PlatformManager m_platformManager;

    inline static const std::string m_salt = "ofQ08azDkAOE6FXl";
    inline static const std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=";
    inline static const int KEY_SIZE = 32;
    inline static const int PASSWORD_SIZE = 16;
};


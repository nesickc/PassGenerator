#pragma once
#include <string>
#include <vector>
#include <array>

#include "PlatformManager.hpp"



class PassGenerator
{
public:
    PassGenerator(const std::string& seed);
    std::string GeneratePassword( const std::string& targetName );
    const PlatformManager& GetPlatformManager() { return m_platformManager; }

private:
    std::vector<unsigned char> GenerateKey( const std::string& seed ) const;

    // checks if the password complies with security standard, i.e. if has all the symbols
    // such as upper case, lower case, number, special characters
    bool CheckPasswordSecurity( const std::string& password ) const;

    std::string combineAlphabetGroups();
    std::string GeneratePasswordInner(std::string targetName, size_t nonce, std::string& password) const;

private:
    std::vector<unsigned char> m_key;
    PlatformManager m_platformManager;

    inline static const std::string m_salt = "ofQ08azDkAOE6FXl";

    const std::vector<std::string> ALPHABET_GROUPS {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0123456789",
        "!@#$%^&*()_+-="
    };

    const std::string m_alphabet = combineAlphabetGroups();

    inline static const int KEY_SIZE = 32;
    inline static const int PASSWORD_SIZE = 16;
};


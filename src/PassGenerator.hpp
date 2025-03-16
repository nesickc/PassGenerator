#pragma once
#include <string>
#include <vector>
#include <array>

#include "PlatformManager.hpp"



class PassGenerator
{
public:
    PassGenerator(std::string seed);
    std::string GeneratePassword( std::string targetName );
    const PlatformManager& GetPlatformManager() { return m_platformManager; }

private:
    std::vector<unsigned char> GenerateKey( std::string seed );

    // checks if the password complies with security standard, i.e. if has all the symbols
    // such as upper case, lower case, number, special characters
    bool checkPasswordSecurity( const std::string& password );

    static constexpr char* combineAlphabetGroups();
    static constexpr size_t calculateAlphabetLength();
private:
    std::vector<unsigned char> m_key;
    PlatformManager m_platformManager;

    inline static const std::string m_salt = "ofQ08azDkAOE6FXl";

    static constexpr size_t ALPHABET_GROUPS_COUNT = 4;
    static inline constexpr std::array<char*, ALPHABET_GROUPS_COUNT> alphabetGroups {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0123456789",
        "!@#$%^&*()_+-="
    };
    inline static const std::string alphabet = combineAlphabetGroups();
    
    inline static const int KEY_SIZE = 32;
    inline static const int PASSWORD_SIZE = 16;
};


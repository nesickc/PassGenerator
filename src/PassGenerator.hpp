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

    std::string GeneratePasswordImpl(std::string targetName, size_t retry);

    // checks if the password complies with security standard, i.e. if has all the symbols
    // such as upper case, lower case, number, special characters
    bool CheckPasswordSecurity( const std::string& password );

    static std::string CombineAlphabetGroups();
    static size_t CalculateAlphabetLength();
private:
    std::vector<unsigned char> m_key;
    PlatformManager m_platformManager;

    inline static const std::string m_salt = "ofQ08azDkAOE6FXl";

    static constexpr size_t ALPHABET_GROUPS_COUNT = 4;
    static inline std::array<std::string, ALPHABET_GROUPS_COUNT> alphabetGroups {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0123456789",
        "!@#$%^&*()_+-="
    };
    inline static const std::string alphabet = CombineAlphabetGroups();
    
    inline static const int KEY_SIZE = 32;
    inline static const int PASSWORD_SIZE = 16;
};


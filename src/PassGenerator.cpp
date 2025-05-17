#include "PassGenerator.hpp"
#include <sodium.h>
#include <set>
#include <map>

#include <stdexcept>

PassGenerator::PassGenerator(const std::string& seed) :
    m_key(GenerateKey(seed)),
    m_platformManager(m_key)
{
    if (sodium_init() == -1)
    {
        throw std::runtime_error("Sodium init problem");
    }

    // this check is necessary because the password is considered secure only if it has characters from all the alphabet groups
    if (ALPHABET_GROUPS.size() > PASSWORD_SIZE) {
        throw std::runtime_error("Alphabet groups size is greater than password size");
    }
}

std::string PassGenerator::GeneratePassword(const std::string& targetName)
{
    m_platformManager.AddPlatform(targetName);
    std::string password;
    for (size_t i = 0; !CheckPasswordSecurity(password); i++)
    {
        password = GeneratePasswordImpl(targetName, i);
    }
    m_platformManager.AddPlatform(targetName);
    return password;
}

std::vector<unsigned char> PassGenerator::GenerateKey( std::string seed )
{
    std::vector<unsigned char> key;
    key.resize( KEY_SIZE );

    // Hash the password using Argon2id
    int res = crypto_pwhash(
        key.data(), key.size(),
        seed.c_str(), seed.size(),
        reinterpret_cast<const unsigned char*>(m_salt.c_str()),
        crypto_pwhash_OPSLIMIT_SENSITIVE,  // Operational cost
        crypto_pwhash_MEMLIMIT_SENSITIVE,  // Memory cost
        crypto_pwhash_ALG_ARGON2ID13       // Argon2id variant
    );

    if ( res != 0 )
    {
        throw std::runtime_error( "Password hashing failed" );
    }

    return key;
}

std::string PassGenerator::GeneratePasswordImpl(std::string targetName, size_t retry)
{
    targetName = targetName + std::to_string(retry);
    std::string password;
    password.reserve(PASSWORD_SIZE);

    // Hash the target name using Argon2id
    std::vector<unsigned char> targetHash(m_key.size());
    bool res = crypto_pwhash(
        targetHash.data(), targetHash.size(),
        targetName.c_str(), targetName.size(),
        reinterpret_cast<const unsigned char*>(m_salt.c_str()),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,  // Operational cost
        crypto_pwhash_MEMLIMIT_INTERACTIVE,  // Memory cost
        crypto_pwhash_ALG_ARGON2ID13       // Argon2id variant
    );

    if (res != 0)
    {
        throw std::runtime_error("Password hashing failed");
    }

    // XOR the two hashes to generate the password
    for (size_t i = 0; i < m_key.size(); i += 2)
    {
        uint32_t passChar1 = m_key[i] ^ targetHash[i];
        uint32_t passChar2 = m_key[i + 1] ^ targetHash[i + 1];
        size_t alphabetIndex = (passChar1 + passChar2) % alphabet.size();
        password.push_back(alphabet[alphabetIndex]);
    }

    return password;
}

bool PassGenerator::CheckPasswordSecurity( const std::string& password )
{
    std::map<const std::string*, bool> groupsUsed;

    for (const auto& charGroup : alphabetGroups)
    {
        groupsUsed[&charGroup] = false;
    }

    for (size_t i = 0; i < password.size(); i++)
    {
        for (const auto& charGroup : alphabetGroups)
        {
            auto pos = std::find(charGroup.begin(), charGroup.end(), password[i]);
            if (pos != charGroup.end())
            {
                groupsUsed[&charGroup] = true;
            }
        }
    }

    bool result = true;
    for (const auto& [charGroup, used] : groupsUsed) {
        result &= used;
    }

    return result;
}

size_t PassGenerator::CalculateAlphabetLength()
{
    size_t length = 0;
    for ( size_t i = 0; i < ALPHABET_GROUPS_COUNT; i++ )
    {
        length += alphabetGroups.size();
    }
    return length;
}


std::string PassGenerator::CombineAlphabetGroups()
{
    size_t alphabetLength = CalculateAlphabetLength();

    std::string tempAlphabet = "";

    for ( const auto& group : alphabetGroups )
    {
        tempAlphabet += group;
    }

    return alphabet;
}

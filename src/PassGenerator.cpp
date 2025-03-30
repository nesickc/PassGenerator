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
    password.reserve(PASSWORD_SIZE);
    GeneratePasswordInner(targetName, 0, password);

    return password;
}

std::string PassGenerator::GeneratePasswordInner(std::string targetName, size_t nonce, std::string& password) const
{
    // adding the nonce to the target name, so that if the password for the targetName is not secure enough,
    // we can add the nonce and try again
    targetName += std::to_string(nonce);

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
        size_t alphabetIndex = (passChar1 + passChar2 + nonce) % m_alphabet.size();
        password.push_back(m_alphabet[alphabetIndex]);
    }

    // check if the password is secure enough, i.e. has characters from all the alphabet groups
    if (!CheckPasswordSecurity(password))
    {
        return GeneratePasswordInner(targetName, nonce + 1, password);
    }

    return password;
}

std::vector<unsigned char> PassGenerator::GenerateKey(const std::string& seed) const
{
    std::vector<unsigned char> key;
    key.resize(KEY_SIZE);

    // Hash the password using Argon2id
    int res = crypto_pwhash(
        key.data(), key.size(),
        seed.c_str(), seed.size(),
        reinterpret_cast<const unsigned char*>(m_salt.c_str()),
        crypto_pwhash_OPSLIMIT_MODERATE,  // Operational cost
        crypto_pwhash_MEMLIMIT_MODERATE,  // Memory cost
        crypto_pwhash_ALG_ARGON2ID13       // Argon2id variant
    );

    if (res != 0)
    {
        throw std::runtime_error("Password hashing failed");
    }

    return key;
}

bool PassGenerator::CheckPasswordSecurity(const std::string& password) const
{
    std::vector<std::pair<std::set<char>, bool>> alphabetGroups;
    alphabetGroups.resize(ALPHABET_GROUPS.size());

    int counter = 0;
    for (auto& [set, flag] : alphabetGroups)
    {
        set.insert(ALPHABET_GROUPS[counter].begin(), ALPHABET_GROUPS[counter].end());
        flag = false;
    }

    for (char c : password)
    {
        for (auto& [set, flag] : alphabetGroups)
        {
            if (set.find(c) != set.end())
            {
                flag = true;
            }
        }
    }

    for (auto& [set, flag] : alphabetGroups)
    {
        if (!flag)
        {
            return false;
        }
    }

    return true;
}

// Combine groups into one compile-time array.
std::string PassGenerator::combineAlphabetGroups() {
    std::string alphabet;

    for (size_t i = 0; i < ALPHABET_GROUPS.size(); i++) {
        alphabet += ALPHABET_GROUPS[i];
    }

    return alphabet;
}

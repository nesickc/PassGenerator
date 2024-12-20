#include "PassGenerator.hpp"
#include <sodium.h>

#include <stdexcept>

PassGenerator::PassGenerator( std::string seed ) :
    m_seed( seed )
{
    m_key.resize( KEY_SIZE );

    // Hash the password using Argon2id
    int res = crypto_pwhash(
        m_key.data(), m_key.size(),
        m_seed.c_str(), m_seed.size(),
        reinterpret_cast<const unsigned char*>( m_salt.c_str() ),
        crypto_pwhash_OPSLIMIT_SENSITIVE,  // Operational cost
        crypto_pwhash_MEMLIMIT_SENSITIVE,  // Memory cost
        crypto_pwhash_ALG_ARGON2ID13       // Argon2id variant
    );

    if (res != 0 )
    {
        throw std::runtime_error( "Password hashing failed" );
    }
}

std::string PassGenerator::GeneratePassword( std::string targetName )
{
    std::string password;
    password.reserve( PASSWORD_SIZE );

    // Hash the target name using Argon2id
    std::vector<unsigned char> targetHash( m_key.size() );
    bool res = crypto_pwhash(
        targetHash.data(), targetHash.size(),
        targetName.c_str(), targetName.size(),
        reinterpret_cast<const unsigned char*>(m_salt.c_str()),
        crypto_pwhash_OPSLIMIT_INTERACTIVE,  // Operational cost
        crypto_pwhash_MEMLIMIT_INTERACTIVE,  // Memory cost
        crypto_pwhash_ALG_ARGON2ID13       // Argon2id variant
    );

    if ( res != 0 )
    {
        throw std::runtime_error( "Password hashing failed" );
    }

    // XOR the two hashes to generate the password
    for ( size_t i = 0; i < m_key.size(); i += 2 )
    {
        uint32_t passChar1 = m_key[i] ^ targetHash[i];
        uint32_t passChar2 = m_key[i + 1] ^ targetHash[i + 1];
        size_t alphabetIndex = (passChar1 + passChar2) % alphabet.size();
        password.push_back( alphabet[alphabetIndex]);
    }

    return password;
}
#include "PassGenerator.hpp"
#include <sodium.h>

#include <stdexcept>
#include <iostream>
#include <fstream>

PlatformManager::PlatformManager(const std::vector<unsigned char>& key)
    : m_platformKey(GeneratePlatformKey(key))
{
}

std::vector<unsigned char> PlatformManager::GeneratePlatformKey(const std::vector<unsigned char>& key)
{
    if (sodium_init() < 0)
    {
        throw std::runtime_error("libsodium initialization failed");
    }

    if (key.size() != crypto_box_SEEDBYTES)
    {
        throw std::runtime_error("Input key must be exactly " + std::to_string(crypto_box_SEEDBYTES) + " bytes long");
    }

    unsigned char publicKey[crypto_box_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_box_SECRETKEYBYTES];

    // Generate the deterministic key pair based on the input key as a seed
    crypto_box_seed_keypair(publicKey, privateKey, key.data());

    // Store the private key in the vector for later use
    std::vector<unsigned char> privateKeyVec(privateKey, privateKey + crypto_box_SECRETKEYBYTES);

    // Save the public key as a filename-friendly string
    m_platformsFilename = PublicKeyToString(publicKey);

    return privateKeyVec;
}

std::string PlatformManager::PublicKeyToString(const unsigned char* publicKey)
{
    char publicKeyHex[crypto_box_PUBLICKEYBYTES * 2 + 1];
    sodium_bin2hex(publicKeyHex, sizeof(publicKeyHex), publicKey, crypto_box_PUBLICKEYBYTES);
    return std::string(publicKeyHex);
}

void PlatformManager::SavePlatforms()
{
    if (m_platforms.empty())
    {
        throw std::runtime_error("No platforms to save");
    }

    // Serialize the platforms
    std::string serializedData;
    for (const auto& platform : m_platforms)
    {
        serializedData += platform + "\n";
    }

    // Encrypt the serialized data
    std::vector<unsigned char> encryptedData(serializedData.size() + crypto_secretbox_MACBYTES);
    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    randombytes_buf(nonce.data(), nonce.size());

    if (crypto_secretbox_easy(
        encryptedData.data(),
        reinterpret_cast<const unsigned char*>(serializedData.data()),
        serializedData.size(),
        nonce.data(),
        m_platformKey.data()) != 0)
    {
        throw std::runtime_error("Encryption failed");
    }

    // Write nonce and encrypted data to the file
    std::ofstream outFile(m_platformsFilename, std::ios::binary);
    if (!outFile)
    {
        throw std::runtime_error("Failed to open file for writing");
    }

    outFile.write(reinterpret_cast<const char*>(nonce.data()), nonce.size());
    outFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
}

void PlatformManager::LoadPlatforms() const
{
    // Read the file
    std::ifstream inFile(m_platformsFilename, std::ios::binary);
    if (!inFile)
    {
        std::cout << "file doesn't exist\n";
        return;
    }

    // Read the nonce
    std::vector<unsigned char> nonce(crypto_secretbox_NONCEBYTES);
    inFile.read(reinterpret_cast<char*>(nonce.data()), nonce.size());

    // Read the encrypted data
    std::vector<unsigned char> encryptedData(
        std::istreambuf_iterator<char>(inFile), {});

    // Decrypt the data
    std::vector<unsigned char> decryptedData(encryptedData.size() - crypto_secretbox_MACBYTES);
    if (crypto_secretbox_open_easy(
        decryptedData.data(),
        encryptedData.data(),
        encryptedData.size(),
        nonce.data(),
        m_platformKey.data()) != 0)
    {
        throw std::runtime_error("Decryption failed");
    }

    // Deserialize the platforms
    std::string serializedData(decryptedData.begin(), decryptedData.end());
    m_platforms.clear();
    size_t pos = 0;
    while ((pos = serializedData.find('\n')) != std::string::npos)
    {
        m_platforms.push_back(serializedData.substr(0, pos));
        serializedData.erase(0, pos + 1);
    }
}

void PlatformManager::AddPlatform(const std::string& platform)
{
    if (std::find(m_platforms.begin(), m_platforms.end(), platform) != m_platforms.end())
    {
        return;
    }

    m_platforms.push_back(platform);
    SavePlatforms();
}

std::vector<std::string> PlatformManager::GetPlatforms() const
{
    if (m_platforms.size() == 0)
    {
        LoadPlatforms();
    }
    return m_platforms;
}

#pragma once
#include <string>
#include <vector>

class PlatformManager
{
public:
    PlatformManager( const std::vector<unsigned char>& key );

    void SavePlatforms();
    void LoadPlatforms() const;

    void AddPlatform( const std::string& platform );
    std::vector<std::string> GetPlatforms() const;

private:
    std::vector<unsigned char> GeneratePlatformKey( const std::vector<unsigned char>& key );
    std::string PublicKeyToString( const unsigned char* publicKey );

    std::string m_platformsFilename {};
    std::vector<unsigned char> m_platformKey {};
    mutable std::vector<std::string> m_platforms {};

};


#pragma once

#include "Texture.h"

class Atlas : public Texture
{
private:
    unsigned int m_AtlasWidth;   // Blocks per rows
    unsigned int m_AtlasHeight;  // Blocks per columns

public:
    Atlas(const std::string& path, unsigned int AtlasWidth, unsigned int AtlasHeight);
    ~Atlas();

    glm::vec2 GetUVCoords(const glm::vec2& baseUV, unsigned short blockID, int face) const;
    int GetTextureID(unsigned short blockID, int face) const;

    inline int GetWidth() const { return m_AtlasWidth; }
    inline int GetHeight() const { return m_AtlasHeight; }
};
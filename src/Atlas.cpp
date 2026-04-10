#include "Atlas.h"

Atlas::Atlas(const std::string& path, unsigned int atlasWidth, unsigned int atlasHeight)
    : Texture(path), m_AtlasWidth(atlasWidth), m_AtlasHeight(atlasHeight)
{

}

Atlas::~Atlas()
{

}

glm::vec2 Atlas::GetUVCoords(const glm::vec2& baseUV, unsigned short blockID, int face) const
{
    int TexIndex=0;
    switch (blockID)
    {
    case 1:  //Grass
        switch (face)
        {
        case 0: case 1: case 4: case 5: // +X, -X, +Z, -Z (side)
            TexIndex = 1;
            break;
        case 2: // +Y (top)
            TexIndex = 0;
            break;
        case 3: // -Y (bottom)
            TexIndex = 2;
            break;
        }
        break;

    case 2:  //Dirt
        TexIndex = 2;
        break;

    case 3:  //Stone
        TexIndex = 3;
        break;

    case 4: //Tree;
        switch (face)
        {
        case 0: case 1: case 4: case 5: // +X, -X, +Z, -Z (side)
            TexIndex = 5;
            break;
        case 2: case 3: // +Y, -Y (top and bottom)
            TexIndex = 4;
            break;
        }
        break;

    case 5: //Leaves
        TexIndex = 6;
        break;
    }
    float TileWidth = 1.0f / m_AtlasWidth;
    float TileHeight = 1.0f / m_AtlasHeight;

    unsigned int TexX = TexIndex % m_AtlasWidth;
    unsigned int TexY = TexIndex / m_AtlasWidth; // si nécessaire, inverser pour OpenGL bottom-left

    glm::vec2 uv;
    uv.x = baseUV.x * TileWidth + TexX * TileWidth;
    uv.y = baseUV.y * TileHeight + TexY * TileHeight;

    return uv;
}
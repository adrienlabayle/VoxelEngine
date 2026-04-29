#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "Chunk.h"

class World;

class TreeGenerator
{
public:
    TreeGenerator(unsigned int seed, int cellSize = 16);

    void GenerateChunkTrees(Chunk& chunk, World& world);

    inline const unsigned short* GetBlocks() const { return m_Blocks; }
    inline const std::vector<glm::ivec3>& GetOutsideLeaves() const { return m_OutsideLeaves; }

private:
    unsigned int m_Seed;
    int m_CellSize;
    unsigned short m_Blocks[Chunk::m_XSize * Chunk::m_YSize * Chunk::m_ZSize] = { 0 };
    std::vector<glm::ivec3> m_OutsideLeaves;

private:
    int Hash(int x, int z, int i) const;

    void PlaceTree(Chunk& chunk, World& world, int worldX, int worldZ, int hash);

    float RandomFloat(int hash) const;
};
#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "Chunk.h"

class World;

class TreeGenerator
{
public:
    TreeGenerator(unsigned int seed, int cellSize = 16);

    void GenerateChunkTrees(int chunkX, int chunkZ, int treeLevel, std::vector<int>& heightTable);

    inline const std::vector<unsigned short>& GetBlocks() const { return m_Blocks; }
    inline const std::vector<glm::ivec3>& GetOutsideLeaves() const { return m_OutsideLeaves; }

private:
    unsigned int m_Seed;
    int m_CellSize;
    std::vector<unsigned short> m_Blocks;
    std::vector<glm::ivec3> m_OutsideLeaves;

private:
    int Hash(int x, int z, int i) const;

    void PlaceTree(std::vector<int>& heightTable, int worldX, int worldZ, int hash);

    float RandomFloat(int hash) const;
};
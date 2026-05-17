#pragma once

#include "glm/glm.hpp"

#include <vector>

#include "Chunk.h"
#include "TerrainGenerator.h"
#include "TreeMaker.h"

class World;

class TreeGenerator
{
public:
    TreeGenerator(unsigned int seed, int cellSize = 16);

    //void GenerateChunkTrees(int chunkX, int chunkZ, int treeLevel, std::vector<int>& heightTable);
    void GenerateChunkTrees(int chunkX, int chunkZ, int* treeLevel, TerrainGenerator& terrainGenerator);
    /*
    inline const std::vector<unsigned short>& GetBlocks() const { return m_Blocks; }
    inline const std::vector<glm::ivec3>& GetOutsideLeaves() const { return m_OutsideLeaves; }
    */
    inline const std::vector<unsigned char>& GetNeighorsBlocks() const { return m_NeighborsBlocks; }
    inline const std::vector<unsigned short>& GetBlocks() const { return m_Blocks; }

private:
    unsigned int m_Seed;
    int m_CellSize;
    TreeMaker m_TreeMaker;
    /*
    std::vector<unsigned short> m_Blocks;
    std::vector<glm::ivec3> m_OutsideLeaves;
    */
    std::vector<unsigned char> m_NeighborsBlocks;
    std::vector<unsigned short> m_Blocks;

private:
    int Hash(int x, int z, int i) const;
    int DeriveHash(int h, int salt) const;

    //void PlaceTree(std::vector<int>& heightTable, int worldX, int worldZ, int hash);
    void PlaceTree(int chunkX, int chunkZ, int localChunkX, int localChunkZ, int chunkQuarter, int hash, TerrainGenerator& terrainGenerator);

    float RandomFloat(int hash) const;
};
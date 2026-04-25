#include "TreeGenerator.h"

#include "World.h"

#include <iostream>

TreeGenerator::TreeGenerator(unsigned int seed, int cellSize)
    : m_Seed(seed), m_CellSize(cellSize)
{

}

int TreeGenerator::Hash(int x, int z) const
{
    int h = x * 374761393 + z * 668265263 + m_Seed;
    h = (h ^ (h >> 13)) * 1274126177;
    return h ^ (h >> 16);
}

float TreeGenerator::RandomFloat(int h) const
{
    return (h & 0xFFFF) / (float)0xFFFF;
}

void TreeGenerator::GenerateChunkTrees(Chunk& chunk, World& world)
{
    int chunkStartX = chunk.GetXWorldPos() * Chunk::m_XSize;
    int chunkStartZ = chunk.GetZWorldPos() * Chunk::m_ZSize;

    int chunkEndX = chunkStartX + Chunk::m_XSize;
    int chunkEndZ = chunkStartZ + Chunk::m_ZSize;

    // cellules couvertes par le chunk
    int minCellX = std::floor(chunkStartX / (float)m_CellSize);
    int maxCellX = std::floor(chunkEndX / (float)m_CellSize);

    int minCellZ = std::floor(chunkStartZ / (float)m_CellSize);
    int maxCellZ = std::floor(chunkEndZ / (float)m_CellSize);

    for (int cz = minCellZ; cz <= maxCellZ; cz++)
    {
        for (int cx = minCellX; cx <= maxCellX; cx++)
        {
            int h = Hash(cx, cz);

            // densité
            if (h % 6 != 0)
                continue;

            float ox = RandomFloat(h);
            float oz = RandomFloat(h >> 16);

            int worldX = cx * m_CellSize + (int)(ox * m_CellSize);
            int worldZ = cz * m_CellSize + (int)(oz * m_CellSize);

            PlaceTree(chunk, world, worldX, worldZ, h);
        }
    }
}

void TreeGenerator::PlaceTree(Chunk& chunk, World& world, int x, int z, int hash)
{
    int groundY = world.GetHeight(x, z);
    std::cout << groundY;

    if (groundY <= 0) return;

    int height = 4 + (hash % 3); // 4–6 blocs

    int localX = (x % Chunk::m_XSize + Chunk::m_XSize) % Chunk::m_XSize; // We avoid negative values by doing this
    int localZ = (z % Chunk::m_ZSize + Chunk::m_ZSize) % Chunk::m_ZSize;

    // Trunk
    for (int y = groundY; y < groundY + height; y++)
    {
        std::cout << "arbre" << "x : " << x << "y : " << y << "z : " << z << std::endl;
        m_Blocks[localX + Chunk::m_XSize * (y + Chunk::m_YSize * localZ)] = 11;  // Wood
    }

    // Leaves (simple cube for start)
    int radius = 2;

    for (int dx = -radius; dx <= radius; dx++)
    {
        for (int dz = -radius; dz <= radius; dz++)
        {
            for (int dy = -radius; dy <= radius; dy++)
            {
                int dist = dx * dx + dy * dy + dz * dz;

                if (dist <= radius * radius)
                {
                    glm::ivec3 pos = { localX + dx, groundY + height + dy, localZ + dz };

                    if (pos[0] >= Chunk::m_XSize || pos[0] < 0 || pos[2] >= Chunk::m_ZSize || pos[2] < 0) // not in the current chunk
                        m_OutsideLeaves.emplace_back( x + dx, pos[1], z + dz);
                    else
                        m_Blocks[pos[0] + Chunk::m_XSize * (pos[1] + Chunk::m_YSize * pos[2])] = 12;  // Leaves
                }
            }
        }
    }
}

#include "TreeGenerator.h"

#include "World.h"

#include <iostream>

TreeGenerator::TreeGenerator(unsigned int seed, int cellSize)
    : m_Seed(seed), m_CellSize(cellSize)
{

}

int TreeGenerator::Hash(int x, int z, int i) const
{
    int h =  x * 19349663 + z * 83492791 + i * 73856093 + m_Seed;
    h = (h ^ (h >> 13)) * 1274126177;
    return h ^ (h >> 16);
}

float TreeGenerator::RandomFloat(int h) const  // Between 0 and 1
{
    return (h & 0xFFFF) / (float)0xFFFF;
}

void TreeGenerator::GenerateChunkTrees(Chunk& chunk, World& world)
{
    // We cut the chunk in a 4 by 4 cells grid, then we gonna fill the chunk with pseudo random points and we want to know how mush heach cells have points
    int CellScore[4 * 4] = { 0 };

    // We fill the grid with our pseudo random points
    for (int i = 0; i < std::pow(chunk.GetTreeLevel(), 2); i++) // We add a number of points proportionally to the TreeLevel of the chunk
    {
        int hx = Hash(chunk.GetXWorldPos(), chunk.GetZWorldPos(), i);
        int hz = Hash(chunk.GetXWorldPos(), chunk.GetZWorldPos(), i + 1337);

        float pointX = RandomFloat(hx);
        float pointZ = RandomFloat(hz);

        int cellX = std::min(3, (int)(pointX * 4));
        int cellZ = std::min(3, (int)(pointZ * 4));

        CellScore[cellX * 4 + cellZ]++;  // std::min(3, (int)(pt * 4)) : [0, 1] -> [0, 3]; points ->cells
    }

    // Now we place the trees in heach cells according to their points score
    int h = Hash(chunk.GetXWorldPos(), chunk.GetZWorldPos(), 0);
    for(int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            switch (CellScore[i * 4 + j])
            {
            case 0: // if point score is 0 then the cell will not have any tree
                break;
            case 1:
                PlaceTree(chunk, world, i * 4 + 1, j * 4 + 1, h); // if it is 1 then we put one normal tree in the middle top left of the cell 
                break;
            case 2:
                PlaceTree(chunk, world, i * 4 + 2, j * 4 + 1, h); // middle top right
                break;
            case 3:
                PlaceTree(chunk, world, i * 4 + 2, j * 4 + 2, h); // middle bottom right
                break;
            case 4:
                PlaceTree(chunk, world, i * 4 + 1, j * 4 + 2, h); // middle bottom left
                break;
            default:    // if the score is >= 5 we put a big tree in the middle of the cell
                PlaceTree(chunk, world, i * 4 + 1, j * 4 + 1, h);
                PlaceTree(chunk, world, i * 4 + 2, j * 4 + 1, h);
                PlaceTree(chunk, world, i * 4 + 2, j * 4 + 2, h);
                PlaceTree(chunk, world, i * 4 + 1, j * 4 + 2, h);
                break;
            }
        }
}

void TreeGenerator::PlaceTree(Chunk& chunk, World& world, int x, int z, int hash)
{
    int groundY = chunk.GetHeight(x, z);

    if (groundY <= 0) return;

    int height = 4 + (hash % 3); // 4–6 blocs

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
                    glm::ivec3 pos = { x + dx, groundY + 1 + height + dy, z + dz };

                    if (pos[0] >= Chunk::m_XSize || pos[0] < 0 || pos[2] >= Chunk::m_ZSize || pos[2] < 0) // not in the current chunk
                        m_OutsideLeaves.emplace_back( x + dx, pos[1], z + dz);
                    else
                        m_Blocks[pos[0] + Chunk::m_XSize * (pos[1] + Chunk::m_YSize * pos[2])] = 12;  // Leaves
                }
            }
        }
    }

    // Trunk
    for (int y = groundY + 1; y <= groundY + height; y++)
    {
        m_Blocks[x + Chunk::m_XSize * (y + Chunk::m_YSize * z)] = 11;  // Wood
    }
}

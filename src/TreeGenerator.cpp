#include "TreeGenerator.h"

#include "World.h"

#include <iostream>

TreeGenerator::TreeGenerator(unsigned int seed, int cellSize)
    : m_Seed(seed), m_CellSize(cellSize)
{
    m_Blocks.resize(Chunk::m_XSize * Chunk::m_YSize * Chunk::m_ZSize, 0);
    m_NeighborsBlocks.resize(Chunk::m_XSize * 3 * Chunk::m_YSize * Chunk::m_ZSize * 3, 0);

    m_TreeMaker = TreeMaker();
}

int TreeGenerator::Hash(int x, int z, int i) const
{
    int h =  x * 19349663 + z * 83492791 + i * 73856093 + m_Seed;
    h = (h ^ (h >> 13)) * 1274126177;
    return h ^ (h >> 16);
}

int TreeGenerator::DeriveHash(int h, int salt) const
{
    h ^= salt;
    h *= 0x9e3779b9;
    h ^= (h >> 16);
    h *= 0x85ebca6b;
    h ^= (h >> 13);
    h *= 0xc2b2ae35;
    h ^= (h >> 16);
    return h;
}

float TreeGenerator::RandomFloat(int h) const  // Between 0 and 1
{
    return (h & 0xFFFF) / (float)0xFFFF;
}
/*
void TreeGenerator::GenerateChunkTrees(int chunkX, int chunkZ, int treeLevel, std::vector<int>& heightTable)
{
    // We cut the chunk in a 4 by 4 cells grid, then we gonna fill the chunk with pseudo random points and we want to know how mush heach cells have points
    int CellScore[4 * 4] = { 0 };

    // We fill the grid with our pseudo random points
    for (int i = 0; i < std::pow(treeLevel, 2); i++) // We add a number of points proportionally to the TreeLevel of the chunk
    {
        int hx = Hash(chunkX, chunkZ, i);
        int hz = Hash(chunkX, chunkZ, i + 1337);

        float pointX = RandomFloat(hx);
        float pointZ = RandomFloat(hz);

        int cellX = std::min(3, (int)(pointX * 4));
        int cellZ = std::min(3, (int)(pointZ * 4));

        CellScore[cellX * 4 + cellZ]++;  // std::min(3, (int)(pt * 4)) : [0, 1] -> [0, 3]; points ->cells
    }

    // Now we place the trees in heach cells according to their points score
    int h = Hash(chunkX, chunkZ, 0);
    for(int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            switch (CellScore[i * 4 + j])
            {
            case 0: // if point score is 0 then the cell will not have any tree
                break;
            case 1:
                PlaceTree(heightTable, i * 4 + 1, j * 4 + 1, h); // if it is 1 then we put one normal tree in the middle top left of the cell 
                break;
            case 2:
                PlaceTree(heightTable, i * 4 + 2, j * 4 + 1, h); // middle top right
                break;
            case 3:
                PlaceTree(heightTable, i * 4 + 2, j * 4 + 2, h); // middle bottom right
                break;
            case 4:
                PlaceTree(heightTable, i * 4 + 1, j * 4 + 2, h); // middle bottom left
                break;
            default:    // if the score is >= 5 we put a big tree in the middle of the cell
                PlaceTree(heightTable, i * 4 + 1, j * 4 + 1, h);
                PlaceTree(heightTable, i * 4 + 2, j * 4 + 1, h);
                PlaceTree(heightTable, i * 4 + 2, j * 4 + 2, h);
                PlaceTree(heightTable, i * 4 + 1, j * 4 + 2, h);
                break;
            }
        }
}

void TreeGenerator::PlaceTree(std::vector<int>& heightTable, int x, int z, int hash)
{
    int groundY = heightTable[x + Chunk::m_XSize * z];

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
*/

void TreeGenerator::GenerateChunkTrees(int chunkX, int chunkZ, int* treeLevels, TerrainGenerator& terrainGenerator)
{
    for (int z = 0; z < 3; z++)
        for (int x = 0; x < 3; x++)
            for (int i = 0; i < 4; i++)
            {
                int h = Hash(chunkX - 1 + x, chunkZ - 1 + z, i);
                float p = RandomFloat(h);

                if (p < treeLevels[z + 3 * x] / 20.0f) // Higher the TreeLevel is, higher the probability of having a tree in this chunk quarter is
                    PlaceTree(chunkX, chunkZ, x, z, i, h, terrainGenerator);
            }
    /*
    for (int z = 0; z < Chunk::m_ZSize; z++)
        for (int y = 0; y < Chunk::m_YSize; y ++)
            for (int x = 0; x < Chunk::m_XSize; x++)
            {
                int Y = heightTable[x + Chunk::m_XSize * z] + 1 + y;
                m_Blocks[x + Chunk::m_XSize * (Y + Chunk::m_YSize * z)] = m_NeighborsBlocks[(1 * Chunk::m_XSize + x) + (Chunk::m_XSize * 3) * (y + 40 * (1 * Chunk::m_ZSize + z))];
            }
    */
}

void TreeGenerator::PlaceTree(int chunkX, int chunkZ, int localChunkX, int localChunkZ, int chunkQuarter, int hash, TerrainGenerator& terrainGenerator) //chunkQuarter 0 = topleft quarter of the (x, z) chunk; 1 = topright; 2 = bottomright; ...
{
    // First we determine the position of the tree in the chunk quarter 
    int positionHash = DeriveHash(hash, 104729);
    int treePosition = positionHash & 15;  // This way we get 16 possibilities for the center of our tree in the i'th chunk quarter(we give choice only between the 4*4 center blocks of the 8*8 blocks quarter of the chunk)
    
    int x = localChunkX * Chunk::m_XSize;  // which column am I according only to the X-axis(considere here as the horizontal axis) chunk position in the 3*3 chunks grid
    int worldX = (chunkX + localChunkX - 1) * Chunk::m_XSize;
    if (chunkQuarter == 1 || chunkQuarter == 2)  // 1 or 2 <=> a right quarter
    {
        x += 8;  // skip 8 columns to get on the bottoms quarter
        worldX += 8;
    }
    x += 2; // skip 2 to get on the center of a quarter(we want to be in the 4*4 center of a quarter(which is 8*8))
    worldX += 2;
    x += treePosition % 4; // here is the real procedural position choice adjustment
    worldX += treePosition % 4;

    int z = localChunkZ * Chunk::m_ZSize;
    int worldZ = (chunkZ + localChunkZ - 1) * Chunk::m_ZSize;
    if (chunkQuarter > 1)  // 2 or 3 <=> a bottom quarter
    {
        z += 8;
        worldZ += 8;
    }
    z += 2;
    worldZ += 2;
    z += treePosition / 4;
    worldZ += treePosition / 4;

    float h = terrainGenerator.GetHeight(worldX, worldZ);
    int baseY = static_cast<int>(h * Chunk::m_YSize + 70);
    baseY = std::clamp(baseY, 0, Chunk::m_YSize - 1);

    x -= Chunk::m_XSize;
    int y = baseY - 1;
    z -= Chunk::m_ZSize;

    m_TreeMaker.MakeTree(m_Blocks, x, y, z, 2, hash);

    /*
    // Then the size of the tree and of the branches
    int sizeHash = DeriveHash(hash, 130363);
    int sizeScore = sizeHash & 3;  // little, normal, big, huge

    int sizeBranchScore = (sizeHash >> 2) & 3;

    int height = sizeScore * 4 + ((sizeHash >> 4) & 3) + 1;  // We determin the height of the tree according to his sizeScore (little = between 1 and 9; normal = 9 and 17; big = 17 and 25; huge = between 25 and 33)
    
    // We determine the position and direction of the branches
    int branchResult = (sizeHash >> 6) & 15;  // Choose who many branch we can have(ex : 0110 means the lowest branch spot will be empty, and the highest also)

    int XAxisBranchesDirection[4] = { (sizeHash >> 10) & 1, (sizeHash >> 11) & 1, (sizeHash >> 12) & 1, (sizeHash >> 13) & 1 };  // XAxisBranchDirection & 1 = 0 means that the 1st branch should be -x oriented, +x if = 1; (X.... >> 1) & 1 for the 2nd branch...
    int ZAxisBranchesDirection[4] = { (sizeHash >> 14) & 1, (sizeHash >> 15) & 1, (sizeHash >> 16) & 1, (sizeHash >> 17) & 1 };

    // We determine the shape of the tree
    int shapeHash = DeriveHash(hash, 155921);
    int shapeScore = shapeHash & 3;  // shapeScore 0 = small amount of leaves around the trunk and branch; 1 = big amount on X axis; 2 = big amount on Z axis; 3 = big amount of leaves on both axis

    float h = terrainGenerator.GetHeight(worldX, worldZ);
    int baseY = static_cast<int>(h * Chunk::m_YSize + 70);
    baseY = std::clamp(baseY, 0, Chunk::m_YSize - 1);

    int XAxisTrunkDirection = ((shapeHash >> 2) & 1) * 2 - 1;  // This way we have -1 or 1, which can be manipulate directly for the offset calcul
    int ZAxisTrunkDirection = ((shapeHash >> 3) & 1) * 2 - 1;

    int maxOffset = std::max(1, height / 3);

    // Here we put the blocks
    for (int y = 1; y <= height; y++)
    {
        int offsetX = 0;
        int offsetZ = 0;
        if (y > 1)
        {
            float t = (float)y / (float)(height - 1);
            float curve = 3.0f * t * t - 2.0f * t * t * t;
            float bend = curve * maxOffset;

            offsetX = (int)std::round(bend * XAxisTrunkDirection);
            offsetZ = (int)std::round(bend * ZAxisTrunkDirection);
        }

        int Y = y + baseY - 1;
        for (int i = -1; i < sizeScore; i++)
            for (int j = -1; j < sizeScore; j++)
            {
                int X = x + offsetX - Chunk::m_XSize + j;
                int Z = z + offsetZ - Chunk::m_ZSize + i;

                if (X >= 0 && (X < Chunk::m_XSize) && y < 40 && Z >= 0 && (Z < Chunk::m_ZSize))
                    m_Blocks[X + Chunk::m_XSize * (Y + Chunk::m_YSize * Z)] = 11;
            }
    }
    */
}

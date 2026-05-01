#include "Generator.h"

#include "World.h"

#include <iostream>

Generator::Generator(World& world, int chunkX, int chunkZ)
	: m_PerlinNoise(world.GetPerlinNoise()), m_VoronoiNoise(world.GetVoronoiNoise()), 
	m_TerrainGenerator(TerrainGenerator(chunkX, chunkZ, m_PerlinNoise, m_VoronoiNoise)), 
	m_TreeGenerator(TreeGenerator(world.GetSeed())), m_ChunkX(chunkX), m_ChunkZ(chunkZ)
{

}

void Generator::GenerateFromChunk()

{
	m_TerrainGenerator.InitBiomes();

	for (int x = 0; x < Chunk::m_XSize; x++)
		for (int z = 0; z < Chunk::m_ZSize; z++)
		{
			int globalX = x + m_ChunkX * Chunk::m_XSize;
			int globalZ = z + m_ChunkZ * Chunk::m_ZSize;

			// Height
			float h = m_TerrainGenerator.GetHeight(globalX, globalZ);
			int height = static_cast<int>(h * Chunk::m_YSize + 70);
			height = std::clamp(height, 0, Chunk::m_YSize - 1);

			m_HeightTable[x + Chunk::m_XSize * z] = height;

			// Biome
			BiomeProfile biome = m_TerrainGenerator.GetBiomeProfile(globalX, globalZ);

			// Block fill
			for (int y = height; y >= 0; --y)
			{
				int index = x + Chunk::m_XSize * (y + Chunk::m_YSize * z);
				if (y == height)
					m_Blocks[index] = biome.topBlock;
				else if (y >= height - 5)
					m_Blocks[index] = biome.layer1;
				else
					m_Blocks[index] = biome.layer2;
			}
		}

	// We take the tree level of the chunks central block as the global tree level of the chunk 
	m_TreeLevel = m_TerrainGenerator.GetBiomeProfile(8 + m_ChunkX * Chunk::m_XSize, 8 + m_ChunkZ * Chunk::m_ZSize).treeLevel;

	m_TreeGenerator.GenerateChunkTrees(m_ChunkX, m_ChunkZ, m_TreeLevel, m_HeightTable);

	const unsigned short* TreesBlocks = m_TreeGenerator.GetBlocks();

	int size = Chunk::m_XSize * Chunk::m_YSize * Chunk::m_ZSize;
	for (int i = 0; i < size; i++)
	{
		if (TreesBlocks[i] != 0)
			m_Blocks[i] = TreesBlocks[i];
	}
}

const unsigned short* Generator::GetBlocks() const
{
	return m_Blocks;
}

const int* Generator::GetHeights() const
{
	return m_HeightTable;
}

const int Generator::GetTreeLevel() const
{
	return m_TreeLevel;
}

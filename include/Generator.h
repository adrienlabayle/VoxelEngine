#pragma once

#include "TerrainGenerator.h"
#include "TreeGenerator.h"
#include "Chunk.h"

class World;

class Generator
{
public:
	Generator(World& world, int ChunkX, int ChunkZ);

	void GenerateFromChunk();

	const unsigned short* GetBlocks() const;
	const int* GetHeights() const;
	const int GetTreeLevel() const;

private:
	Noise::PerlinNoise m_PerlinNoise;
	Noise::VoronoiNoise m_VoronoiNoise;

	TerrainGenerator m_TerrainGenerator;
	TreeGenerator m_TreeGenerator;

	int m_ChunkX;
	int m_ChunkZ;

	unsigned short m_Blocks[Chunk::m_XSize * Chunk::m_YSize * Chunk::m_ZSize] = { 0 };
	int m_HeightTable[Chunk::m_XSize * Chunk::m_ZSize] = { 0 };
	int m_TreeLevel = 0;
};

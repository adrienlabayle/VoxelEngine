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

	inline const std::vector<unsigned short>& GetBlocks() const { return m_Blocks; }
	inline const std::vector<int>& GetHeights() const { return m_HeightTable; }

	inline std::vector<unsigned short>&& MoveBlocks() { return std::move(m_Blocks); }
	inline std::vector<int>&& MoveHeights() { return std::move(m_HeightTable); }
	inline const int GetTreeLevel() const { return m_TreeLevel; }

private:
	Noise::PerlinNoise m_PerlinNoise;
	Noise::VoronoiNoise m_VoronoiNoise;

	TerrainGenerator m_TerrainGenerator;
	TreeGenerator m_TreeGenerator;

	int m_ChunkX;
	int m_ChunkZ;

	std::vector<unsigned short> m_Blocks;
	std::vector<int> m_HeightTable;

	int m_TreeLevel = 0;
};

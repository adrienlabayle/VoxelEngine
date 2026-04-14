#pragma once

#include "PerlinNoise.h"
#include <cmath>
#include <algorithm>

struct BiomeProfile
{
	float (*heightFunc)(float base);

	int topBlock;
	int layer1;
	int layer2;

	bool hasTrees;
};

class TerrainGenerator
{
public:

	// Constructor
	TerrainGenerator(
		int chunkX,
		int chunkZ,
		const Noise::PerlinNoise& noise,
		float baseFreq = 0.01f,
		float detailFreq = 0.05f,
		float biomeFreq = 0.005f,
		float biomeSharpness = 5.0f,
		float blendWidth = 100.0f
	);

	// Init
	static void InitBiomes();

	// Core
	float GetHeight(int x, int z);
	BiomeProfile GetBiomeProfile(int x, int z);

private:

	// Data
	int m_ChunkX;
	int m_ChunkZ;

	const Noise::PerlinNoise& m_Noise;

	// Config
	float m_BaseFreq;
	float m_DetailFreq;
	float m_BiomeFreq;
	float m_BiomeSharpness;
	float m_GlobalBlendWidth;

	// Biomes
	static BiomeProfile m_Biomes[4][4]; // [global][inter]

	// Internal
	int GetGlobalBiome(int x, int z);

	float GetInterBiomeNoise(int x, int z);

	void ComputeInterWeights(float n, float weights[4]);

	void ComputeGlobalWeights(int x, int z, float weights[4]);
};
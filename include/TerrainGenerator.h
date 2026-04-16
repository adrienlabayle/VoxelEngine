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
		float globalBiomeFreq = 0.0005f,
		float interBiomeFreq = 0.003f,
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
	float m_GlobalBiomeFreq;
	float m_InterBiomeFreq;
	float m_BiomeSharpness;
	float m_GlobalBlendWidth;

	// Biomes
	static BiomeProfile m_Biomes[4][4]; // [global][inter]

	// Internal
	float GetGlobalBiomeNoise(int x, int z);

	float GetInterBiomeNoise(int x, int z);

	void ComputeWeights(float n, float weights[4]);
};
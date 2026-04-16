#include "TerrainGenerator.h"

// HEIGHT FUNCTIONS

static float Plains(float base)
{
	return base;
}

static float Hills(float base)
{
	return std::pow(base, 0.9f);
}

static float Mountains(float base)
{
	return std::pow(base, 0.7f) * 1.5;
}

static float Flat(float base)
{
	return base * 0.3f;
}

BiomeProfile TerrainGenerator::m_Biomes[4][4]; // Static biome table

// INIT BIOMES

void TerrainGenerator::InitBiomes()
{
	// GLOBAL 0 (plains / forest)
	m_Biomes[0][0] = { Plains,     1, 2, 3, true };
	m_Biomes[0][1] = { Hills,      9, 2, 3, true };
	m_Biomes[0][2] = { Hills,      9, 2, 3, true };
	m_Biomes[0][3] = { Mountains,  10, 3, 3, false };

	// GLOBAL 1 (desert)
	m_Biomes[1][0] = { Flat,       4, 4, 3, false };
	m_Biomes[1][1] = { Plains,     4, 4, 3, false };
	m_Biomes[1][2] = { Hills,      7, 4, 3, false };
	m_Biomes[1][3] = { Mountains,  8, 3, 3, false };

	// GLOBAL 2 (snow)
	m_Biomes[2][0] = { Plains,     6, 2, 3, false };
	m_Biomes[2][1] = { Hills,      5, 2, 3, false };
	m_Biomes[2][2] = { Mountains,  9, 3, 3, false };
	m_Biomes[2][3] = { Mountains,  10, 3, 3, false };

	// GLOBAL 3 (mountain region)
	m_Biomes[3][0] = { Plains,     3, 2, 3, true };
	m_Biomes[3][1] = { Hills,      3, 2, 3, true };
	m_Biomes[3][2] = { Mountains,  3, 3, 3, false };
	m_Biomes[3][3] = { Mountains,  3, 3, 3, false };
}

TerrainGenerator::TerrainGenerator(int chunkX, int chunkZ,const Noise::PerlinNoise& noise, float baseFreq, float detailFreq, float globalBiomeFreq, float interBiomeFreq, float biomeSharpness, float blendWidth)
	: m_ChunkX(chunkX), m_ChunkZ(chunkZ), m_Noise(noise), m_BaseFreq(baseFreq), m_DetailFreq(detailFreq), m_GlobalBiomeFreq(globalBiomeFreq), 
	m_InterBiomeFreq(interBiomeFreq), m_BiomeSharpness(biomeSharpness), m_GlobalBlendWidth(blendWidth)
{

}

// Inter biome noise
float TerrainGenerator::GetInterBiomeNoise(int x, int z)
{
	return m_Noise.Perlin(x * m_InterBiomeFreq, z * m_InterBiomeFreq);
}

// Global biome noise
float TerrainGenerator::GetGlobalBiomeNoise(int x, int z)
{
	return m_Noise.Perlin(x * m_GlobalBiomeFreq, z * m_GlobalBiomeFreq);
}

// Compute weights

void TerrainGenerator::ComputeWeights(float n, float weights[4])
{
	float centers[4] = { 0.2f, 0.4f, 0.6f, 0.8f };

	float sum = 0.0f;

	for (int i = 0; i < 4; i++)
	{
		float d = std::abs(n - centers[i]);

		weights[i] = 1.0f - d * m_BiomeSharpness;
		weights[i] = std::clamp(weights[i], 0.0f, 1.0f);

		sum += weights[i];
	}

	if (sum > 0.0f)
	{
		for (int i = 0; i < 4; i++)
			weights[i] /= sum;
	}
}

// HEIGHT

float TerrainGenerator::GetHeight(int x, int z)
{
	// Base terrain
	float base =
		m_Noise.Perlin(x * m_BaseFreq, z * m_BaseFreq);

	float detail =
		m_Noise.Perlin(x * m_DetailFreq, z * m_DetailFreq);

	base = base * 0.2f + detail * 0.05f; // octave 1 + octave 2
	base *= 0.5f;

	// Inter biome noise and weights
	float interNoise = GetInterBiomeNoise(x, z);

	float interWeights[4];
	ComputeWeights(interNoise, interWeights);

	// Global biome noise and weights
	float globalNoise = GetGlobalBiomeNoise(x, z);

	float globalWeights[4];
	ComputeWeights(globalNoise, globalWeights);

	float height = 0.0f;

	// Double blending: global × inter
	for (int g = 0; g < 4; g++)
	{
		for (int i = 0; i < 4; i++)
		{
			BiomeProfile& biome = m_Biomes[g][i];

			float h = biome.heightFunc(base);

			height += h * globalWeights[g] * interWeights[i];
		}
	}

	return height;
}

// BIOME PROFILE (for blocks)

BiomeProfile TerrainGenerator::GetBiomeProfile(int x, int z)
{
	// Inter biome noise and weights
	float interNoise = GetInterBiomeNoise(x, z);

	float interWeights[4];
	ComputeWeights(interNoise, interWeights);

	// Global biome noise and weights
	float globalNoise = GetGlobalBiomeNoise(x, z);

	float globalWeights[4];
	ComputeWeights(globalNoise, globalWeights);

	// ADD the block mixt effect in the borders between two global biomes
	// Add small noise ONLY to the two non-zero weights
	for (int g = 0; g < 4; g++)
	{
		if (globalWeights[g] > 0.01f)
		{
			float GlobalBorderNoise = m_Noise.Perlin((x + 750 + g * 100) * 0.5f, (z + 750 + g * 100) * 0.5f); // *0.5f = frequence
			GlobalBorderNoise -= 0.5f; // centered noise

			globalWeights[g] += GlobalBorderNoise * 0.05f; // amplitude, so frequence and amplitude will define here the visual transition between txo global biome
			globalWeights[g] = std::max(globalWeights[g], 0.0f);
		}
	}

	// Normalisation
	float sum = 0.0f;
	for (int g = 0; g < 4; g++) sum += globalWeights[g];

	if (sum > 0.0f)
	{
		for (int g = 0; g < 4; g++)
			globalWeights[g] /= sum;
	}

	// Find dominant global biome
	int bestGlobal = 0;
	float maxG = globalWeights[0];

	for (int g = 1; g < 4; g++)
	{
		if (globalWeights[g] > maxG)
		{
			maxG = globalWeights[g];
			bestGlobal = g;
		}
	}

	// Find dominant inter biome
	int bestInter = 0;
	float maxI = interWeights[0];

	for (int i = 1; i < 4; i++)
	{
		if (interWeights[i] > maxI)
		{
			maxI = interWeights[i];
			bestInter = i;
		}
	}

	return m_Biomes[bestGlobal][bestInter];
}
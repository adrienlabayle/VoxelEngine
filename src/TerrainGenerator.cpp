#include "TerrainGenerator.h"

// HEIGHT FUNCTIONS

static float Plains(float base)
{
	return base;
}

static float Hills(float base)
{
	return std::pow(base, 1.3f);
}

static float Mountains(float base)
{
	return std::pow(base, 2.0f) * 1.5f;
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
	m_Biomes[0][1] = { Hills,      1, 2, 3, true };
	m_Biomes[0][2] = { Hills,      1, 2, 3, true };
	m_Biomes[0][3] = { Mountains,  3, 3, 3, false };

	// GLOBAL 1 (desert)
	m_Biomes[1][0] = { Flat,       4, 4, 3, false };
	m_Biomes[1][1] = { Plains,     4, 4, 3, false };
	m_Biomes[1][2] = { Hills,      4, 4, 3, false };
	m_Biomes[1][3] = { Mountains,  3, 3, 3, false };

	// GLOBAL 2 (snow)
	m_Biomes[2][0] = { Plains,     5, 2, 3, false };
	m_Biomes[2][1] = { Hills,      5, 2, 3, false };
	m_Biomes[2][2] = { Mountains,  5, 3, 3, false };
	m_Biomes[2][3] = { Mountains,  5, 3, 3, false };

	// GLOBAL 3 (mountain region)
	m_Biomes[3][0] = { Plains,     1, 2, 3, true };
	m_Biomes[3][1] = { Hills,      1, 2, 3, true };
	m_Biomes[3][2] = { Mountains,  3, 3, 3, false };
	m_Biomes[3][3] = { Mountains,  3, 3, 3, false };
}

TerrainGenerator::TerrainGenerator(int chunkX, int chunkZ,const Noise::PerlinNoise& noise, float baseFreq, float detailFreq, float biomeFreq, float biomeSharpness, float blendWidth)
	: m_ChunkX(chunkX), m_ChunkZ(chunkZ), m_Noise(noise), m_BaseFreq(baseFreq), m_DetailFreq(detailFreq), 
	m_BiomeFreq(biomeFreq), m_BiomeSharpness(biomeSharpness), m_GlobalBlendWidth(blendWidth)
{

}

// Globale biome (fixe areas)

int TerrainGenerator::GetGlobalBiome(int x, int z)
{
	if (x >= 0 && z >= 0) return 0;
	if (x < 0 && z >= 0) return 1;
	if (x < 0 && z < 0) return 2;
	return 3;
}

// Inter biome noise

float TerrainGenerator::GetInterBiomeNoise(int x, int z)
{
	return m_Noise.Perlin(x * m_BiomeFreq, z * m_BiomeFreq);
}

// Compute weights

void TerrainGenerator::ComputeInterWeights(float n, float weights[4])
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

// Compute global biome weights based on distance to axes

void TerrainGenerator::ComputeGlobalWeights(int x, int z, float weights[4])
{
	float d[4];
	float sum = 0.0f;

	// If the point is already in the biome, the distance is turn negativ else the distance must be positiv, this way we get a signed distance
	if (x >= 0 && z >= 0) // If I am in the Biome 0 (top right)
	{
		d[0] = (-1) * std::min(x, z); // Distance to closest border of biome 0 (top right), equivalent to the minimum distance between the x and z axis
		d[1] = x; // Distance to closest border of biome 1 (top left), equivalent to the distance from the z axis
		d[2] = std::sqrt(x * x + z * z); // Distance to closest border of biome 2 (bottom left), equivalent to distance from the origin(0, 0)
		d[3] = z; // Distance to closest border of biome 3 (bottom right), equivalent to the distance from the x axis
	}
	else if (x < 0 && z >= 0) // If I am in the Biome 1 (top left)
	{
		d[0] = -x;
		d[1] = (-1) * std::min(-x, z);
		d[2] = z;
		d[3] = std::sqrt(x * x + z * z);
	}
	else if (x < 0 && z < 0) // If I am in the Biome 2 (bottom left)
	{
		d[0] = std::sqrt(x * x + z * z);
		d[1] = -z;
		d[2] = (-1) * std::min(-x, -z);;
		d[3] = -x;
	}
	else // If I am in the Biome 3 (bottom right)
	{
		d[0] = -z;
		d[1] = std::sqrt(x * x + z * z);
		d[2] = x;
		d[3] = (-1) * std::min(x, -z);
	}

	for (int i = 0; i < 4; i++)
	{
		float w;
		if (d[i] < 0)  // If we have a negative distance then we want the distance to represente directly the weight
			w = 0.5 + (-d[i] / m_GlobalBlendWidth) * 0.5; // tends to 0.5 near the bordern tends to 1 far the border
		else
			w = 0.5f - (d[i] / m_GlobalBlendWidth) * 0.5f; // tends to 0.5 near the bordern tends to 0 far the border

		w = std::clamp(w, 0.0f, 1.0f);
		weights[i] = w;
		sum += w;
	}

	// Normalize
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

	base = base * 0.8f + detail * 0.2f; // octave 1 + octave 2

	// Inter biome
	float n = GetInterBiomeNoise(x, z);

	float interWeights[4];
	ComputeInterWeights(n, interWeights);

	// Global biome weights
	float globalWeights[4];
	ComputeGlobalWeights(x, z, globalWeights);

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
	float n = GetInterBiomeNoise(x, z);

	float interWeights[4];
	ComputeInterWeights(n, interWeights);

	float globalWeights[4];
	ComputeGlobalWeights(x, z, globalWeights);

	// Add small noise ONLY to the two non-zero weights
	for (int g = 0; g < 4; g++)
	{
		if (globalWeights[g] > 0.01f)
		{
			float GlobalBorderNoise = m_Noise.Perlin((x + 750 + g * 100) * 0.01f, (z + 750 + g * 100) * 0.01f);
			GlobalBorderNoise -= 0.5f; // centered noise

			globalWeights[g] += GlobalBorderNoise * 0.1f; // low amplitude
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
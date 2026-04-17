#include "VoronoiNoise.h"

namespace Noise {

    VoronoiNoise::VoronoiNoise(unsigned int seed)
        : m_Seed(seed)
    {

    }

    // Etape 1 : Le Hash

    int VoronoiNoise::Hash(int x, int z) const
    {
        int h = x * 374761393 + z * 668265263; // grands nombres premiers
        h = (h ^ (h >> 13)) * 1274126177;
        return h ^ (h >> 16);
    }

    // Etape 2 : Centroid dans chaque cellule

    std::pair<float, float> VoronoiNoise::GetCentroid(int cellX, int cellZ) const
    {
        int h = Hash(cellX, cellZ);

        // Génère 2 floats pseudo random dans [0,1]
        float offsetX = (h & 0xFFFF) / (float)0xFFFF;
        float offsetZ = ((h >> 16) & 0xFFFF) / (float)0xFFFF;

        return {
            cellX + offsetX,
            cellZ + offsetZ
        };
    }

    // Etape 3 : Multi-centroid + biomes + blending

    // A : implementation
    void VoronoiNoise::GetBiomes(float x, float z, float weights[4]) const
    {
        int cellX = (int)std::floor(x);
        int cellZ = (int)std::floor(z);

        std::vector<Candidate> candidates;

        // On check une zone 3x3 (souvent suffisant)
        for (int dz = -1; dz <= 1; dz++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                int cx = cellX + dx;
                int cz = cellZ + dz;

                auto [px, pz] = GetCentroid(cx, cz);

                float dxp = px - x;
                float dzp = pz - z;
                float dist = dxp * dxp + dzp * dzp;

                int h = Hash(cx, cz);
                int biome = h & 3; // 4 biomes

                candidates.push_back({ dist, biome });
            }
        }

        // B : definir les poids des 4 biomes différents
        float bestDist[4];

        // init
        for (int i = 0; i < 4; i++)
        {
            bestDist[i] = FLT_MAX;
            weights[i] = 0.0f;
        }

        // trouver le meilleur centroid pour chaque biome
        for (const auto& c : candidates)
        {
            if (c.dist < bestDist[c.biomeID])
            {
                bestDist[c.biomeID] = c.dist;
            }
        }

        // calcule des poids
        float sum = 0.0f;

        for (int i = 0; i < 4; i++)
        {
            if (bestDist[i] < FLT_MAX)
            {
                weights[i] = 1.0f / std::pow(bestDist[i] + 0.0001f, 1.5f);
                sum += weights[i];
            }
        }

        // C : normalisation
        if (sum > 0.0f)
        {
            for (int i = 0; i < 4; i++)
            {
                weights[i] /= sum;
            }
        }
    }
}



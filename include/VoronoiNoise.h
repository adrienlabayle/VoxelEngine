#pragma once

#include <algorithm>   // std::sort
#include <cmath>       // std::floor, std::sqrt
#include <cfloat>      // FLT_MAX
#include <utility>     // std::pair

namespace Noise {

    struct Candidate
    {
        float dist;
        int biomeID;
    };

    class VoronoiNoise
    {
    public:
        VoronoiNoise(unsigned int seed);

        void GetBiomes(float x, float z, float weights[4]) const;

    private:
        unsigned int m_Seed;

        int Hash(int x, int z) const;
        std::pair<float, float> GetCentroid(int cellX, int cellZ) const;
    };
}



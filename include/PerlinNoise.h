#pragma once

#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

namespace Noise {

	class PerlinNoise
	{
	public:
		PerlinNoise(unsigned int Seed);

		void InitPermutation();
		float Fade(float t) const;
		float Lerp(float t, float a, float b) const;
		float Grad(int hash, float x, float y) const;
		float Perlin(float x, float y) const;

	private:
		std::vector<int> m_Permutation;
		unsigned int m_Seed;
	};
}

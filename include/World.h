#pragma once

#include <vector>
#include <functional>
#include <string>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "Chunk.h"
#include "Mesh.h"
#include "Atlas.h"
#include "TerrainGenerator.h"
#include "TreeGenerator.h"
#include "Frustum.h"
#include "ThreadSafeQueue.h"

#include <thread>
#include <atomic>

enum JobType{ GenerateJobType, MeshJobType };

struct ChunkPosition
{
	int x;
	int z;

	bool operator==(const ChunkPosition& other) const
	{
		return x == other.x && z == other.z;
	}
};

struct ChunkPositionHash
{
	std::size_t operator()(const ChunkPosition& pos) const
	{
		std::size_t h1 = std::hash<int>()(pos.x);
		std::size_t h2 = std::hash<int>()(pos.z);

		return h1 ^ (h2 << 1); // shift + xor
	}
};

struct WorkerJob
{
	JobType type;

	ChunkPosition pos;

	// Generate

	// Mesh
	std::shared_ptr<Chunk> center;
	std::shared_ptr<Chunk> left;
	std::shared_ptr<Chunk> right;
	std::shared_ptr<Chunk> front;
	std::shared_ptr<Chunk> back;
};

struct WorkerResult
{
	JobType type;

	ChunkPosition pos;

	// Generate
	unsigned short m_Blocks[Chunk::m_XSize * Chunk::m_YSize * Chunk::m_ZSize] = { 0 };

	// Mesh
	std::vector<Vertex> opaqueVertices;
	std::vector<unsigned int> opaqueIndices;

	std::vector<Vertex> transparentVertices;
	std::vector<unsigned int> transparentIndices;
};

struct MeshJob
{
	ChunkPosition pos;

	std::shared_ptr<Chunk> center;
	std::shared_ptr<Chunk> left;
	std::shared_ptr<Chunk> right;
	std::shared_ptr<Chunk> front;
	std::shared_ptr<Chunk> back;
};

struct MeshResult
{
	ChunkPosition pos;

	std::vector<Vertex> opaqueVertices;
	std::vector<unsigned int> opaqueIndices;

	std::vector<Vertex> transparentVertices;
	std::vector<unsigned int> transparentIndices;
};

class World
{
public:
	World(int RenderDistance, std::shared_ptr<Atlas> Atlas, unsigned int Seed);
	~World();

	void Load(const glm::vec3& CameraChunkPosition);
	void Draw(const glm::vec3& CameraChunkPosition, const Shader* Shader, const glm::mat4& View, const glm::mat4& Proj);
	void WorkerLoop();

	int GetHeight(int x, int z) const;

	inline const unsigned int GetSeed() const { return m_Seed; }

	inline const Noise::PerlinNoise& GetPerlinNoise() const { return m_PerlinNoise; }
	inline const Noise::VoronoiNoise& GetVoronoiNoise() const { return m_VoronoiNoise; }

private:
	std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>, ChunkPositionHash> m_Chunks;
	std::unique_ptr<Renderer> m_Renderer;

	std::shared_ptr<Atlas> m_Texture;

	int m_RenderDistance;
	unsigned int m_Seed;

	glm::vec2 m_LastCamChunkPos = { INT_MAX, INT_MAX };

	Noise::PerlinNoise m_PerlinNoise;
	Noise::VoronoiNoise m_VoronoiNoise;

	std::vector<std::shared_ptr<Chunk>> m_OrderedChunks;
	int m_MaxRemeshPerFrame = 2; // 2 chunks per frame max

	ThreadSafeQueue<MeshJob> m_MeshQueue;
	ThreadSafeQueue<MeshResult> m_ResultQueue;

	std::vector<std::thread> m_Workers;
	std::atomic<bool> m_Stopped = false;
};
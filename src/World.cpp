#include "World.h"

World::World(int RenderDistance, std::shared_ptr<Atlas> Atlas, unsigned int Seed)
	: m_RenderDistance(RenderDistance), m_Texture(Atlas), m_PerlinNoise(Seed)
{
    m_Renderer = std::make_unique<Renderer>();

	int threadCount = std::thread::hardware_concurrency();
	if (threadCount == 0)
		threadCount = 4; // fallback

	for (int i = 0; i < threadCount; i++)
	{
		m_Workers.emplace_back(&World::WorkerLoop, this);
	}
}

World::~World()
{
	m_Stopped = true;

	m_JobQueue.Stop();     // Wake up workers
	m_ResultQueue.Stop();

	for (auto& t : m_Workers)
		if (t.joinable())
			t.join();
}

void World::Load(const glm::vec3& CameraChunkPosition)
{
	int ChunkX = floor(CameraChunkPosition.x / Chunk::m_XSize);
	int ChunkZ = floor(CameraChunkPosition.z / Chunk::m_ZSize);

	// Delete far chunks
	for (auto it = m_Chunks.begin(); it != m_Chunks.end(); )
	{
		int dx = it->first.x - ChunkX;
		int dz = it->first.z - ChunkZ;

		if (std::abs(dx) > m_RenderDistance || std::abs(dz) > m_RenderDistance)
		{
			it = m_Chunks.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Create the missing chunks (put them in NeedRemesh state)
	for (int x = -m_RenderDistance; x <= m_RenderDistance; x++)
		for (int z = -m_RenderDistance; z <= m_RenderDistance; z++)
		{
			ChunkPosition pos = { ChunkX + x, ChunkZ + z };

			if (m_Chunks.find(pos) == m_Chunks.end())
			{
				auto newChunk = std::make_shared<Chunk>(pos.x, pos.z, *this);
				m_Chunks[pos] = newChunk;

				// Put the existing neighbors in "NeedRemesh" state (lambda)
				auto markNeighbor = [&](int nx, int nz)
					{
						ChunkPosition p{ nx, nz };
						auto it = m_Chunks.find(p);
						if (it != m_Chunks.end())
							it->second->SetNeedRemesh(true);
					};

				markNeighbor(pos.x - 1, pos.z);
				markNeighbor(pos.x + 1, pos.z);
				markNeighbor(pos.x, pos.z + 1);
				markNeighbor(pos.x, pos.z - 1);

				newChunk->SetNeedRemesh(true);
			}
		}

	// Push jobs (NeedrRemesh -> threads)
	int jobCount = 0;

	for (auto& [pos, chunk] : m_Chunks)
	{
		if (chunk->GetNeedRemesh() && jobCount < m_MaxRemeshPerFrame)
		{
			MeshJob job;
			job.pos = pos;
			job.center = chunk;

			// Get neighbors (lambda)
			auto get = [&](int x, int z) -> std::shared_ptr<Chunk>
				{
					ChunkPosition p{ x, z };
					auto it = m_Chunks.find(p);
					return (it != m_Chunks.end()) ? it->second : nullptr;
				};

			job.left = get(pos.x - 1, pos.z);
			job.right = get(pos.x + 1, pos.z);
			job.front = get(pos.x, pos.z + 1);
			job.back = get(pos.x, pos.z - 1);

			m_JobQueue.Push(std::move(job));

			chunk->SetNeedRemesh(false);
			jobCount++;
		}
	}
}

void World::Draw(const glm::vec3& CameraChunkPosition, const Shader* Shader, const glm::mat4& View, const glm::mat4& Proj)
{
	Load(CameraChunkPosition);

	while (auto result = m_ResultQueue.TryPop())
	{
		auto it = m_Chunks.find(result->pos);
		if (it == m_Chunks.end())
			continue;

		auto& chunk = it->second;

		chunk->ApplyMesh(
			result->opaqueVertices,
			result->opaqueIndices,
			result->transparentVertices,
			result->transparentIndices
		);
	}

	Frustum frustum(Proj, View);

	glm::vec2 CamPos = glm::vec2(
		floor(CameraChunkPosition.x / Chunk::m_XSize),
		floor(CameraChunkPosition.z / Chunk::m_ZSize)
	);

	if (CamPos != m_LastCamChunkPos)
	{
		m_OrderedChunks.clear();

		for (auto& [Pos, chunk] : m_Chunks)
		{
			if (chunk->IsMeshReady())
			{
				float wx = chunk->GetXWorldPos() * Chunk::m_XSize;
				float wz = chunk->GetZWorldPos() * Chunk::m_ZSize;

				if (frustum.IsChunkInFrustum(wx, wz))
					m_OrderedChunks.push_back(chunk);
			}
		}

		std::sort(m_OrderedChunks.begin(), m_OrderedChunks.end(),
			[&](std::shared_ptr<Chunk> a, std::shared_ptr<Chunk> b)
			{
				glm::vec2 dA = glm::vec2(a->GetXWorldPos(), a->GetZWorldPos()) - CamPos;
				glm::vec2 dB = glm::vec2(b->GetXWorldPos(), b->GetZWorldPos()) - CamPos;
				return glm::dot(dA, dA) > glm::dot(dB, dB);
			});

		m_LastCamChunkPos = CamPos;
	}

	// Opaque
	for (auto& [Pos, chunk] : m_Chunks)
	{
		if (chunk->IsMeshReady())
		{
			float wx = chunk->GetXWorldPos() * Chunk::m_XSize;
			float wz = chunk->GetZWorldPos() * Chunk::m_ZSize;

			if (frustum.IsChunkInFrustum(wx, wz))
			{
				m_Renderer->Draw(*chunk->GetOpaqueVertexArray(), *chunk->GetOpaqueIndexBuffer(), *Shader);
			}
		}
	}

	// Transparent
	for (auto& chunk : m_OrderedChunks)
	{
		m_Renderer->Draw(*chunk->GetTransparentVertexArray(), *chunk->GetTransparentIndexBuffer(), *Shader);
	}
}

void World::WorkerLoop()
{
	while (true)
	{
		auto jobOpt = m_JobQueue.Pop(); // BLOQUANT

		// sécurité shutdown (si queue stop)
		if (!jobOpt)
			break;

		auto job = std::move(*jobOpt);

		Mesh mesh;
		mesh.MeshFromChunk(
			m_Texture.get(),
			*job.center,
			job.left.get(),
			job.right.get(),
			job.front.get(),
			job.back.get()
		);

		MeshResult result;
		result.pos = job.pos;
		result.opaqueVertices = mesh.GetOpaqueVertices();
		result.opaqueIndices = mesh.GetOpaqueIndices();
		result.transparentVertices = mesh.GetTransparentVertices();
		result.transparentIndices = mesh.GetTransparentIndices();

		m_ResultQueue.Push(std::move(result));
	}
}
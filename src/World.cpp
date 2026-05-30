#include "World.h"

World::World(int RenderDistance, std::shared_ptr<Atlas> Atlas, unsigned int Seed)
	: m_RenderDistance(RenderDistance), m_Texture(Atlas), m_PerlinNoise(Seed), m_VoronoiNoise(Seed), m_Seed(Seed)
{
    m_Renderer = std::make_unique<Renderer>();

	int threadCount = std::thread::hardware_concurrency();
	if (threadCount == 0)
		threadCount = 4; // fallback

	for (int i = 0; i < threadCount; i++)
	{
		m_Workers.emplace_back(&World::WorkerLoop, this);
	}

	TerrainGenerator::InitBiomes();
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

		if (dx * dx + dz * dz > m_RenderDistance * m_RenderDistance)
		{
			it = m_Chunks.erase(it);
		}
		else
		{
			++it;
		}
	}

	// Create the missing chunks (put them in NeedGenerate state)
	for (int x = -m_RenderDistance; x <= m_RenderDistance; x++)
		for (int z = -m_RenderDistance; z <= m_RenderDistance; z++)
		{
			if (x * x + z * z > m_RenderDistance * m_RenderDistance) continue; // skip the corners

			ChunkPosition pos = { ChunkX + x, ChunkZ + z };

			if (m_Chunks.find(pos) == m_Chunks.end())
			{
				auto newChunk = std::make_shared<Chunk>(pos.x, pos.z, *this);
				m_Chunks[pos] = newChunk;

				newChunk->SetNeedGeneration(true);
			}
		}

	// Push jobs (NeedRemesh -> threads)
	int jobCount = 0;

	for (auto& [pos, chunk] : m_Chunks)
	{
		if (!chunk->GetNeedGeneration() && chunk->GetNeedRemesh() && jobCount < m_MaxRemeshPerFrame)
		{
			WorkerJob job;
			job.type = MeshJobType;
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

	// WE PUT THE GENERATE JOBS AFTER THE MESH JOBS BECAUSE OUR QUEUE ARE LI-FO, AND WE WANT THE GENERATE JOBS TO BE ALLWAYS PRIORITY
	// Push jobs (NeedGenerate -> threads)
	jobCount = 0;

	for (auto& [pos, chunk] : m_Chunks)
	{
		if (chunk->GetNeedGeneration()) // We get read of the m_MaxGeneratePerFrame because we want it to be a priority
		{
			WorkerJob job;
			job.type = GenerateJobType;
			job.pos = pos;

			m_JobQueue.Push(std::move(job));

			chunk->SetNeedGeneration(false);
			jobCount++;
		}
	}
}



void World::Draw(const glm::vec3& CameraChunkPosition, Shader* shader, Shader* SsboShader, const glm::mat4& View, const glm::mat4& Proj)
{
	// LOAD PART : 

	Load(CameraChunkPosition);

	// WORKERS RESULTS MANAGING AND UPLOAD GPU/CPU PART : 

	while (auto result = m_ResultQueue.TryPop())
	{
		auto it = m_Chunks.find(result->pos);
		if (it == m_Chunks.end())
			continue;

		auto& chunk = it->second;

		if (result->type == GenerateJobType)
		{
			chunk->ApplyGenerate(result->blocks, result->heightTable, result->treeLevel);

			// Put the existing neighbors in "NeedRemesh" state (lambda)
			ChunkPosition pos = result->pos;
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

			// Put the current generated chunk in "NeedRemesh" state
			chunk->SetNeedRemesh(true);
		}
		else if (result->type == MeshJobType)
		{
			// If the chunk already had an allocation, we free
			if (chunk->GetMegaSSBOOffset() != UINT32_MAX)
				m_MegaSSBO.Free(chunk->GetMegaSSBOOffset(), chunk->GetMegaSSBOFaceCount());

			// New allocation
			uint32_t faceCount = result->opaqueSSBO.size();
			uint32_t offset = m_MegaSSBO.Allocate(faceCount);

			if (offset == UINT32_MAX) // Here we avoid fragmentation issues in the mega ssbo by stacking all the used data on the start position of the mega ssbo, leaving a single big bloc of place in the other part of the buffer
			{
				CompactMegaSSBO();
				offset = m_MegaSSBO.Allocate(faceCount); // Retry
			}

			m_MegaSSBO.Upload(result->opaqueSSBO.data(), faceCount, offset);
			chunk->SetMegaSSBO(offset, faceCount);

			chunk->ApplyMesh(result->opaqueSSBO, result->opaqueVertices, result->opaqueIndices, result->transparentVertices, result->transparentIndices);
		}
	}

	// FRUSTUM AND MESHED CHUNKS ORDERING PART : 

	Frustum frustum(Proj, View);

	glm::vec2 CamPos = glm::vec2(floor(CameraChunkPosition.x / Chunk::m_XSize), floor(CameraChunkPosition.z / Chunk::m_ZSize));

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

	// DRAWING PART : 

	// Opaque - indirect drawing
	m_DrawIndirectBuffer.Clear();

	for (auto& [Pos, chunk] : m_Chunks)
	{
		if (!chunk->IsMeshReady()) continue;

		float wx = chunk->GetXWorldPos() * Chunk::m_XSize;
		float wz = chunk->GetZWorldPos() * Chunk::m_ZSize;

		if (!frustum.IsChunkInFrustum(wx, wz)) continue;
		if (chunk->GetMegaSSBOFaceCount() == 0) continue;

		// Encode chunkX and chunkZ in 'baseInstance' (16 firt bits for chunkX, those bits are unsigned so we have to add 32768 to endle that, 16 next bits are for chunZ)
		uint32_t baseInstance = ((uint32_t)(chunk->GetXWorldPos() + 32768) & 0xFFFF) | (((uint32_t)(chunk->GetZWorldPos() + 32768) & 0xFFFF) << 16);

		m_DrawIndirectBuffer.AddCommand(chunk->GetMegaSSBOFaceCount(), chunk->GetMegaSSBOOffset(), baseInstance);
	}
	m_DrawIndirectBuffer.Upload();
	if (m_DrawIndirectBuffer.GetCommandCount() == 0)
	{
		m_DrawIndirectBuffer.Bind();
		return;
	}

	m_Renderer->Draw(m_EmptyVAO, m_MegaSSBO, *SsboShader, m_DrawIndirectBuffer);

	/*
	// Opaque
	for (auto& [Pos, chunk] : m_Chunks)
	{
		if (chunk->IsMeshReady())
		{
			float wx = chunk->GetXWorldPos() * Chunk::m_XSize;
			float wz = chunk->GetZWorldPos() * Chunk::m_ZSize;

			if (frustum.IsChunkInFrustum(wx, wz))
			{
				// Pipeline A
				SsboShader->Bind();
				SsboShader->SetUniform3f("u_ChunkWorldPos", wx, 0.0f, wz);
				m_Renderer->Draw(*chunk->GetOpaqueEmptyVAO(), *chunk->GetOpaqueSSBO(), *SsboShader, chunk->GetOpaqueSSBOIndexCount());

				// Pipeline B
				//m_Renderer->Draw(*chunk->GetOpaqueVertexArray(), *chunk->GetOpaqueIndexBuffer(), *shader);
			}
		}
	}
	*/

	// Transparent
	for (auto& chunk : m_OrderedChunks)
	{
		//m_Renderer->Draw(*chunk->GetTransparentVertexArray(), *chunk->GetTransparentIndexBuffer(), *shader);
	}
}

void World::WorkerLoop()
{
	while (true)
	{
		auto jobOpt = m_JobQueue.Pop(); // BLOQUANT
		
		// securite shutdown (si queue stop)
		if (!jobOpt)
			break;

		auto job = std::move(*jobOpt);

		if (job.type == GenerateJobType)
		{
			Generator generator(*this, job.pos.x, job.pos.z);
			generator.GenerateFromChunk();

			WorkerResult result;
			result.type = GenerateJobType;
			result.pos = job.pos;
			result.blocks = generator.MoveBlocks();
			result.heightTable = generator.MoveHeights();
			result.treeLevel = generator.GetTreeLevel();

			m_ResultQueue.Push(std::move(result));
		}
		else if (job.type == MeshJobType)
		{
			Mesh mesh;
			mesh.MeshFromChunk(m_Texture.get(), *job.center, job.left.get(), job.right.get(), job.front.get(), job.back.get());

			WorkerResult result;
			result.type = MeshJobType;
			result.pos = job.pos;
			result.opaqueSSBO = mesh.GetOpaqueSSBO();
			result.opaqueVertices = mesh.GetOpaqueVertices();
			result.opaqueIndices = mesh.GetOpaqueIndices();
			result.transparentVertices = mesh.GetTransparentVertices();
			result.transparentIndices = mesh.GetTransparentIndices();

			m_ResultQueue.Push(std::move(result));
		}
	}
}

// Dans World
void World::CompactMegaSSBO()
{
	uint32_t writeOffset = 0;

	for (auto& [pos, chunk] : m_Chunks)
	{
		if (chunk->GetMegaSSBOOffset() == UINT32_MAX) continue;
		if (chunk->GetMegaSSBOFaceCount() == 0) continue;

		uint32_t oldOffset = chunk->GetMegaSSBOOffset();
		uint32_t faceCount = chunk->GetMegaSSBOFaceCount();

		if (oldOffset != writeOffset)
			m_MegaSSBO.Move(oldOffset, writeOffset, faceCount); // glCopyBufferSubData

		chunk->SetMegaSSBO(writeOffset, faceCount);
		writeOffset += faceCount;
	}

	m_MegaSSBO.ResetFreeList(writeOffset); // put back one single free bloc
}

int World::GetHeight(int x, int z) const
{
	int XChunkPos = (int)std::floor(x / (float)Chunk::m_XSize);
	int ZChunkPos = (int)std::floor(z / (float)Chunk::m_ZSize);

	ChunkPosition pos = { XChunkPos, ZChunkPos };

	if (m_Chunks.find(pos) != m_Chunks.end())
	{
		int localX = (x % Chunk::m_XSize + Chunk::m_XSize) % Chunk::m_XSize;
		int localZ = (z % Chunk::m_ZSize + Chunk::m_ZSize) % Chunk::m_ZSize;

		auto& chunk = m_Chunks.at(pos);

		return chunk->GetHeight(localX, localZ);
	}
	return -1;
}

#include "Chunk.h"

#include "Mesh.h"
#include "World.h"

Chunk::Chunk(int XWorldPos, int ZWorldPos, World& World)
	: m_XWordPos(XWorldPos), m_ZWordPos(ZWorldPos)
{
	Generate(World);
}

Chunk::~Chunk()
{

}

void Chunk::Generate(World& World)
{
	TerrainGenerator generator(m_XWordPos, m_ZWordPos, World.GetPerlinNoise(), World.GetVoronoiNoise());

	generator.InitBiomes();

	for (int x = 0; x < m_XSize; x++)
		for (int z = 0; z < m_ZSize; z++)
		{
			int globalX = x + m_XWordPos * m_XSize;
			int globalZ = z + m_ZWordPos * m_ZSize;

			// Height
			float h = generator.GetHeight(globalX, globalZ);
			int height = static_cast<int>(h * m_YSize + 70);

			height = std::clamp(height, 0, m_YSize - 1);

			m_HeightTable[x + m_XSize * z] = height;

			// Biome
			BiomeProfile biome = generator.GetBiomeProfile(globalX, globalZ);

			// Block fill
			for (int y = height; y >= 0; --y)
			{
				int index = x + m_XSize * (y + m_YSize * z);

				if (y == height)
					m_Blocks[index] = biome.topBlock;

				else if (y >= height - 5)
					m_Blocks[index] = biome.layer1;

				else
					m_Blocks[index] = biome.layer2;
			}
		}
	// We take the tree level of the chunks central block as the global tree level of the chunk 
	SetTreeLevel(generator.GetBiomeProfile(8 + m_XWordPos * m_XSize, 8 + m_ZWordPos * m_ZSize).treeLevel); 

	TreeGenerator treeGenerator(World.GetSeed());

	treeGenerator.GenerateChunkTrees(*this, World);

	ApplyGeneration(treeGenerator.GetBlocks());
}

void Chunk::ApplyGeneration(const unsigned short* blocks) // can potentially be optimized
{
	int size = m_XSize * m_YSize * m_ZSize;

	for (int i = 0; i < size; i++)
	{
		if (blocks[i] != 0)
			m_Blocks[i] = blocks[i];
	}
}

void Chunk::ApplyMesh(const std::vector<Vertex>& opaqueV, const std::vector<unsigned int>& opaqueI, const std::vector<Vertex>& transparentV, const std::vector<unsigned int>& transparentI)
{
	VertexBufferLayout layout;
	layout.PushVertex();

	// Opaque
	m_OpaqueVertexArray = std::make_unique<VertexArray>();
	m_OpaqueVertexBuffer = std::make_unique<VertexBuffer>(opaqueV.data(), opaqueV.size() * sizeof(Vertex));
	m_OpaqueVertexArray->AddBuffer(*m_OpaqueVertexBuffer, layout);
	m_OpaqueIndexBuffer = std::make_unique<IndexBuffer>(opaqueI.data(), opaqueI.size());

	// Transparent
	m_TransparentVertexArray = std::make_unique<VertexArray>();
	m_TransparentVertexBuffer = std::make_unique<VertexBuffer>(transparentV.data(), transparentV.size() * sizeof(Vertex));
	m_TransparentVertexArray->AddBuffer(*m_TransparentVertexBuffer, layout);
	m_TransparentIndexBuffer = std::make_unique<IndexBuffer>(transparentI.data(), transparentI.size());

	m_Loaded = true;
}

// Getters and setters

unsigned short Chunk::GetBlockLocal(int x, int y, int z) const
{
	if (x >= 0 && x < m_XSize &&
		y >= 0 && y < m_YSize &&
		z >= 0 && z < m_ZSize)
	{
		return m_Blocks[x + m_XSize * (y + m_YSize * z)];
	}

	return 0; // air
}

int Chunk::GetHeight(int x, int z) const
{
	return m_HeightTable[x + m_XSize * z];
}

void Chunk::SetNeedGeneration(bool state)
{
	m_NeedGeneration = state;
}

bool Chunk::GetNeedGeneration() const
{
	return m_NeedGeneration;
}

void Chunk::SetNeedRemesh(bool state)
{
	m_NeedRemesh = state;
}

bool Chunk::GetNeedRemesh() const
{
	return m_NeedRemesh;
}

void Chunk::SetTreeLevel(int level)
{
	m_TreeLevel = level;
}

int Chunk::GetTreeLevel() const
{
	return m_TreeLevel;
}

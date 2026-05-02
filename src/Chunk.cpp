#include "Chunk.h"

#include "Mesh.h"
#include "World.h"

#include "Generator.h"

Chunk::Chunk(int XWorldPos, int ZWorldPos, World& World)
	: m_XWordPos(XWorldPos), m_ZWordPos(ZWorldPos)
{
	//Generate(World);
}

Chunk::~Chunk()
{

}

void Chunk::Generate(World& World) // Used before multithreading implementation, useless now
{
	Generator generator(World, m_XWordPos, m_ZWordPos);
	generator.GenerateFromChunk();
	ApplyGenerate(generator.GetBlocks(), generator.GetHeights(), generator.GetTreeLevel());
}

void Chunk::ApplyGenerate(const std::vector<unsigned short>& blocks, const std::vector<int>& heightTable, const int treeLevel) // can potentially be optimized
{
	memcpy(m_Blocks, blocks.data(), sizeof(unsigned short) * m_XSize * m_YSize * m_ZSize);
	memcpy(m_HeightTable, heightTable.data(), sizeof(int) * Chunk::m_XSize * Chunk::m_ZSize);
	m_TreeLevel = treeLevel;
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

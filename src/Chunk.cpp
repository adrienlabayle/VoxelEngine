#include "Chunk.h"

#include "Mesh.h"
#include "World.h"

Chunk::Chunk(int XWorldPos, int ZWorldPos, World& World)
	: m_XWordPos(XWorldPos), m_ZWordPos(ZWorldPos)
{
	for (int x = 0; x < m_XSize; x++)
		for (int z = 0; z < m_ZSize; z++)
		{
			int globalX = x + m_XWordPos * m_XSize;
			int globalZ = z + m_ZWordPos * m_ZSize;

			float baseFreq = 1.0f / 64.0f;
			float biomeFreq = 1.0f / 128.0f;
			float canyonFreq = 1.0f / 32.0f;

			float amplitude = 0.07f;

			float base = World.GetPerlinNoise().Perlin(globalX * baseFreq, globalZ * baseFreq);
			float biome = World.GetPerlinNoise().Perlin(globalX * biomeFreq + 750, globalZ * biomeFreq + 750);
			float canyon = World.GetPerlinNoise().Perlin(globalX * canyonFreq + 1500, globalZ * canyonFreq + 1500);

			// montagnes
			float mountain = pow(base, 2.0f);

			// canyon
			float canyonShape = std::abs(canyon - 0.5f) * 2.0f;

			// mélange
			float biomeFactor = std::clamp((biome - 0.5f) * 2.0f, 0.0f, 1.0f);
			base = base * (1.0f - biomeFactor) + mountain * biomeFactor;
			base -= canyonShape * 0.3f;

			base = std::clamp(base, 0.0f, 1.0f);
			int h = static_cast<int>(base * amplitude * m_YSize + 70);  //static_cast<type>(...) <=> moderne de (type)(...)

			for (int y = h; y >= 0; --y) {
				if (y == h)
					m_Blocks[x + m_XSize * (y + m_YSize * z)] = 1; // Grass
				else if (y >= std::max(h - 5, 0))
					m_Blocks[x + m_XSize * (y + m_YSize * z)] = 2; // Dirt
				else
					m_Blocks[x + m_XSize * (y + m_YSize * z)] = 3; // Stone
			}
		}
}

Chunk::~Chunk()
{
	if (m_OpaqueIndexBuffer) delete m_OpaqueIndexBuffer;
	if (m_OpaqueVertexBuffer) delete m_OpaqueVertexBuffer;
	if (m_OpaqueVertexArray) delete m_OpaqueVertexArray;

	if (m_TransparentIndexBuffer) delete m_TransparentIndexBuffer;
	if (m_TransparentVertexBuffer) delete m_TransparentVertexBuffer;
	if (m_TransparentVertexArray) delete m_TransparentVertexArray;
}

void Chunk::ApplyMesh(const std::vector<Vertex>& opaqueV, const std::vector<unsigned int>& opaqueI, const std::vector<Vertex>& transparentV, const std::vector<unsigned int>& transparentI)
{
	if (m_Loaded)
	{
		delete m_OpaqueVertexArray;
		delete m_OpaqueVertexBuffer;
		delete m_OpaqueIndexBuffer;

		delete m_TransparentVertexArray;
		delete m_TransparentVertexBuffer;
		delete m_TransparentIndexBuffer;
	}

	VertexBufferLayout layout;
	layout.PushVertex();

	// Opaque
	m_OpaqueVertexArray = new VertexArray();
	m_OpaqueVertexBuffer = new VertexBuffer(opaqueV.data(), opaqueV.size() * sizeof(Vertex));
	m_OpaqueVertexArray->AddBuffer(*m_OpaqueVertexBuffer, layout);
	m_OpaqueIndexBuffer = new IndexBuffer(opaqueI.data(), opaqueI.size());

	// Transparent
	m_TransparentVertexArray = new VertexArray();
	m_TransparentVertexBuffer = new VertexBuffer(transparentV.data(), transparentV.size() * sizeof(Vertex));
	m_TransparentVertexArray->AddBuffer(*m_TransparentVertexBuffer, layout);
	m_TransparentIndexBuffer = new IndexBuffer(transparentI.data(), transparentI.size());

	m_Loaded = true;
}

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

void Chunk::SetNeedRemesh(bool state)
{
	m_NeedRemesh = state;
}

bool Chunk::GetNeedRemesh() const
{
	return m_NeedRemesh;
}

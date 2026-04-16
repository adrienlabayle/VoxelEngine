#include "Mesh.h"

Mesh::Mesh()
{
    m_BlockTable[0].transparent = true;
    //m_BlockTable[5].transparent = true;
}

Mesh::~Mesh()
{
}

unsigned short Mesh::GetBlockSafe(int x, int y, int z, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back) const
{
    // bounds Y
    if (y < 0 || y >= Chunk::m_YSize)
        return 0;

    // center
    if (x >= 0 && x < Chunk::m_XSize &&
        z >= 0 && z < Chunk::m_ZSize)
    {
        return center.GetBlockLocal(x, y, z);
    }

    // left
    if (x == -1)
        return left ? left->GetBlockLocal(Chunk::m_XSize - 1, y, z) : 0;

    // right
    if (x == Chunk::m_XSize)
        return right ? right->GetBlockLocal(0, y, z) : 0;

    // back
    if (z == -1)
        return back ? back->GetBlockLocal(x, y, Chunk::m_ZSize - 1) : 0;

    // front
    if (z == Chunk::m_ZSize)
        return front ? front->GetBlockLocal(x, y, 0) : 0;

    return 0;
}

bool Mesh::IsFaceVisible(int x, int y, int z, int face,
    const Chunk& center,
    const Chunk* left,
    const Chunk* right,
    const Chunk* front,
    const Chunk* back) const
{
    // micro-opt (évite glm)
    static const int dx[6] = { 1,-1,0,0,0,0 };
    static const int dy[6] = { 0,0,1,-1,0,0 };
    static const int dz[6] = { 0,0,0,0,1,-1 };

    unsigned short neighbor = GetBlockSafe(
        x + dx[face],
        y + dy[face],
        z + dz[face],
        center, left, right, front, back
    );

    // safety
    if (neighbor >= 256)
        return true;

    return m_BlockTable[neighbor].transparent;
}

void Mesh::AddFaceVerticesAndIndices(const Chunk& chunk,
    int x, int y, int z,
    int face,
    unsigned short blockID,
    const Atlas* Atlas,
    bool Transparent)
{
    glm::vec3 basePos = { (float)x, (float)y, (float)z };

    static const glm::vec3 faceVertices[6][4] = {
        {{1,0,1},{1,1,1},{1,1,0},{1,0,0}}, // +X
        {{0,0,0},{0,1,0},{0,1,1},{0,0,1}}, // -X
        {{0,1,0},{1,1,0},{1,1,1},{0,1,1}}, // +Y
        {{0,0,1},{1,0,1},{1,0,0},{0,0,0}}, // -Y
        {{0,0,1},{0,1,1},{1,1,1},{1,0,1}}, // +Z
        {{1,0,0},{1,1,0},{0,1,0},{0,0,0}}  // -Z
    };

    glm::vec2 uv[4] = { {0,0}, {0,1}, {1,1}, {1,0} };

    glm::vec3 translation = {
        chunk.GetXWorldPos() * Chunk::m_XSize,
        0,
        chunk.GetZWorldPos() * Chunk::m_ZSize
    };

    if (!Transparent)
    {
        unsigned int startIndex = m_OpaqueVertices.size();

        for (int i = 0; i < 4; i++)
        {
            glm::vec3 pos = basePos + faceVertices[face][i] + translation;
            glm::vec2 texpos = Atlas->GetUVCoords(uv[i], blockID, face);

            Vertex v;
            v.x = pos.x;
            v.y = pos.y;
            v.z = pos.z;
            v.u = texpos.x;
            v.v = texpos.y;

            m_OpaqueVertices.push_back(v);
        }

        m_OpaqueIndices.push_back(startIndex + 0);
        m_OpaqueIndices.push_back(startIndex + 1);
        m_OpaqueIndices.push_back(startIndex + 2);
        m_OpaqueIndices.push_back(startIndex + 2);
        m_OpaqueIndices.push_back(startIndex + 3);
        m_OpaqueIndices.push_back(startIndex + 0);
    }
    else
    {
        unsigned int startIndex = m_TransparentVertices.size();

        for (int i = 0; i < 4; i++)
        {
            glm::vec3 pos = basePos + faceVertices[face][i] + translation;
            glm::vec2 texpos = Atlas->GetUVCoords(uv[i], blockID, face);

            Vertex v;
            v.x = pos.x;
            v.y = pos.y;
            v.z = pos.z;
            v.u = texpos.x;
            v.v = texpos.y;

            m_TransparentVertices.push_back(v);
        }

        m_TransparentIndices.push_back(startIndex + 0);
        m_TransparentIndices.push_back(startIndex + 1);
        m_TransparentIndices.push_back(startIndex + 2);
        m_TransparentIndices.push_back(startIndex + 2);
        m_TransparentIndices.push_back(startIndex + 3);
        m_TransparentIndices.push_back(startIndex + 0);
    }
}

void Mesh::MeshFromChunk(const Atlas* Atlas,
    const Chunk& center,
    const Chunk* left,
    const Chunk* right,
    const Chunk* front,
    const Chunk* back)
{
    m_OpaqueVertices.clear();
    m_OpaqueIndices.clear();

    m_TransparentVertices.clear();
    m_TransparentIndices.clear();

    // Order cache-friendly
    for (int y = 0; y < Chunk::m_YSize; y++)
        for (int z = 0; z < Chunk::m_ZSize; z++)
            for (int x = 0; x < Chunk::m_XSize; x++)
            {
                // OPTI : direct acces
                unsigned short BlockID = center.GetBlockLocal(x, y, z);

                if (BlockID == 0)
                    continue;

                for (int face = 0; face < 6; face++)
                {
                    if (IsFaceVisible(x, y, z, face, center, left, right, front, back))
                    {
                        bool transparent = (BlockID < 256) ? m_BlockTable[BlockID].transparent : true;

                        AddFaceVerticesAndIndices(
                            center,
                            x, y, z,
                            face,
                            BlockID,
                            Atlas,
                            transparent
                        );
                    }
                }
            }
}

const std::vector<Vertex>& Mesh::GetOpaqueVertices() const
{
    return m_OpaqueVertices;
}

const std::vector<unsigned int>& Mesh::GetOpaqueIndices() const
{
    return m_OpaqueIndices;
}

const std::vector<Vertex>& Mesh::GetTransparentVertices() const
{
    return m_TransparentVertices;
}

const std::vector<unsigned int>& Mesh::GetTransparentIndices() const
{
    return m_TransparentIndices;
}
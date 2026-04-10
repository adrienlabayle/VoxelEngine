#pragma once

#include "Renderer.h"

#include <random>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "Vertex.h"
#include "Atlas.h"

class World;

class Chunk
{
public:
    Chunk(int XWorldPos, int ZWorldPos, World& World);
    ~Chunk();

    static const int m_XSize = 16;
    static const int m_YSize = 512;
    static const int m_ZSize = 16;

    void ApplyMesh(const std::vector<Vertex>& opaqueV, const std::vector<unsigned int>& opaqueI, const std::vector<Vertex>& transparentV, const std::vector<unsigned int>& transparentI);

    unsigned short GetBlockLocal(int x, int y, int z) const;

    inline int GetXWorldPos() const { return m_XWordPos; }
    inline int GetZWorldPos() const { return m_ZWordPos; }

    inline VertexArray* GetOpaqueVertexArray() const { return m_OpaqueVertexArray; }
    inline VertexBuffer* GetOpaqueVertexBuffer() const { return m_OpaqueVertexBuffer; }
    inline IndexBuffer* GetOpaqueIndexBuffer() const { return m_OpaqueIndexBuffer; }

    inline VertexArray* GetTransparentVertexArray() const { return m_TransparentVertexArray; }
    inline VertexBuffer* GetTransparentVertexBuffer() const { return m_TransparentVertexBuffer; }
    inline IndexBuffer* GetTransparentIndexBuffer() const { return m_TransparentIndexBuffer; }

    void SetNeedRemesh(bool state);
    bool GetNeedRemesh() const;

    inline bool IsMeshReady() const { return m_Loaded; }

private:
    unsigned short m_Blocks[m_XSize * m_YSize * m_ZSize] = { 0 }; // init with air
    int m_XWordPos, m_ZWordPos;

    VertexArray* m_OpaqueVertexArray = nullptr;
    VertexBuffer* m_OpaqueVertexBuffer = nullptr;
    IndexBuffer* m_OpaqueIndexBuffer = nullptr;

    VertexArray* m_TransparentVertexArray = nullptr;
    VertexBuffer* m_TransparentVertexBuffer = nullptr;
    IndexBuffer* m_TransparentIndexBuffer = nullptr;

    bool m_Loaded = false;
    bool m_NeedRemesh = false;
};
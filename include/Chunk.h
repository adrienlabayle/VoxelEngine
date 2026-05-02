#pragma once

#include "Renderer.h"

#include <random>
#include <cstring>

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

    void Generate(World& World);
    void ApplyGenerate(const std::vector<unsigned short>& blocks, const std::vector<int>& heightTable, const int treeLevel);
    void ApplyMesh(const std::vector<Vertex>& opaqueV, const std::vector<unsigned int>& opaqueI, const std::vector<Vertex>& transparentV, const std::vector<unsigned int>& transparentI);

    unsigned short GetBlockLocal(int x, int y, int z) const;
    int GetHeight(int x, int z) const;

    inline int GetXWorldPos() const { return m_XWordPos; }
    inline int GetZWorldPos() const { return m_ZWordPos; }

    inline VertexArray* GetOpaqueVertexArray() const { return m_OpaqueVertexArray.get(); }
    inline VertexBuffer* GetOpaqueVertexBuffer() const { return m_OpaqueVertexBuffer.get(); }
    inline IndexBuffer* GetOpaqueIndexBuffer() const { return m_OpaqueIndexBuffer.get(); }

    inline VertexArray* GetTransparentVertexArray() const { return m_TransparentVertexArray.get(); }
    inline VertexBuffer* GetTransparentVertexBuffer() const { return m_TransparentVertexBuffer.get(); }
    inline IndexBuffer* GetTransparentIndexBuffer() const { return m_TransparentIndexBuffer.get(); }

    void SetNeedGeneration(bool state);
    bool GetNeedGeneration() const;

    void SetNeedRemesh(bool state);
    bool GetNeedRemesh() const;

    void SetTreeLevel(int level);
    int GetTreeLevel() const;

    inline bool IsMeshReady() const { return m_Loaded; }

private:
    unsigned short m_Blocks[m_XSize * m_YSize * m_ZSize] = { 0 }; // init with air
    int m_XWordPos, m_ZWordPos;

    std::unique_ptr<VertexArray> m_OpaqueVertexArray = nullptr;
    std::unique_ptr<VertexBuffer> m_OpaqueVertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_OpaqueIndexBuffer = nullptr;

    std::unique_ptr<VertexArray> m_TransparentVertexArray = nullptr;
    std::unique_ptr<VertexBuffer> m_TransparentVertexBuffer = nullptr;
    std::unique_ptr<IndexBuffer> m_TransparentIndexBuffer = nullptr;

    bool m_Loaded = false;
    bool m_NeedGeneration = false;
    bool m_NeedRemesh = false;
    int m_TreeLevel = 0;

    int m_HeightTable[m_XSize * m_ZSize] = { 0 };
};
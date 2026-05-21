#pragma once

#include "Vertex.h"
#include "Chunk.h"
#include "Atlas.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include<iostream>

struct BlockInfo
{
	bool pipelineA = true;
	bool pipelineB = false;
	bool pipelineC = false;

	bool transparent = false;
	bool blended = false;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	unsigned short GetBlockSafe(int x, int y, int z, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back) const;
	unsigned char IsFaceVisible(unsigned short blockID, int x, int y, int z, int face, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back) const;
	void AddFaceVerticesAndIndices(const Chunk& chunk, int x, int y, int z, int face, unsigned short blockID, const Atlas* Atlas, bool Blended);
	void AddPipelineAFaces(const Chunk& chunk, int x, int y, int z, int face, unsigned short blockID, const Atlas* Atlas);
	void MeshFromChunk(const Atlas* Atlas, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back);

	inline const std::vector<PackedVertex>& GetOpaqueSSBO() const { return m_OpaqueSSBO; }
	inline const std::vector<Vertex>& GetOpaqueVertices() const { return m_OpaqueVertices; };
	inline const std::vector<unsigned int>& GetOpaqueIndices() const { return m_OpaqueIndices; };
	inline const std::vector<Vertex>& GetTransparentVertices() const { return m_TransparentVertices; };
	inline const std::vector<unsigned int>& GetTransparentIndices() const { return m_TransparentIndices; };

private:

	//Pipeline A
	std::vector<PackedVertex> m_OpaqueSSBO;

	//Pipeline B
	std::vector<Vertex> m_OpaqueVertices;
	std::vector<unsigned int> m_OpaqueIndices;
	std::vector<Vertex> m_TransparentVertices;
	std::vector<unsigned int> m_TransparentIndices;

	BlockInfo m_BlockTable[256];
};
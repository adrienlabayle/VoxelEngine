#pragma once

#include "Vertex.h"
#include "Chunk.h"
#include "Atlas.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include<iostream>

struct BlockInfo
{
	bool transparent = false;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	unsigned short GetBlockSafe(int x, int y, int z, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back) const;
	bool IsFaceVisible(int x, int y, int z, int face, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back) const;
	void AddFaceVerticesAndIndices(const Chunk& chunk, int x, int y, int z, int face, unsigned short blockID, const Atlas* Atlas, bool Transparent);
	void MeshFromChunk(const Atlas* Atlas, const Chunk& center, const Chunk* left, const Chunk* right, const Chunk* front, const Chunk* back);

	const std::vector<Vertex>& GetOpaqueVertices() const;
	const std::vector<unsigned int>& GetOpaqueIndices() const;
	const std::vector<Vertex>& GetTransparentVertices() const;
	const std::vector<unsigned int>& GetTransparentIndices() const;

private:
	std::vector<Vertex> m_OpaqueVertices;
	std::vector<unsigned int> m_OpaqueIndices;
	std::vector<Vertex> m_TransparentVertices;
	std::vector<unsigned int> m_TransparentIndices;
	BlockInfo m_BlockTable[256];
};
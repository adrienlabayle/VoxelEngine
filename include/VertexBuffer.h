#pragma once

#include<iostream>

class VertexBuffer
{
private:
	unsigned int m_RendererID;

public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Reload(const void* data, unsigned int size) const;

	void Bind() const;
	void Unbind() const;
};
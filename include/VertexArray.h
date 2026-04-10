#pragma once

#include<iostream>

#include "VertexBuffer.h"

class VertexBufferLayout;  // equivalent dun include(avec le include du cpp) mais evite une boucle infini dinclude entre VertexBufferLayout et Renderer

class VertexArray
{
private:
	unsigned int m_RendererID;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind() const;
	void Unbind() const;
};
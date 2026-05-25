#pragma once

#include <glad/glad.h>

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "ShaderStorageBuffer.h"
#include "MegaSSBO.h"
#include "DrawIndirectBuffer.h"

#define ASSERT(x) if (!(x)) __debugbreak();  // ce sont des c++ macros
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
    void Draw(const VertexArray& va, const ShaderStorageBuffer& ssbo, const Shader& shader, int vertexCount) const;
    void Draw(const VertexArray& va, const MegaSSBO& ssbo, const Shader& shader, const DrawIndirectBuffer& dib) const;
};
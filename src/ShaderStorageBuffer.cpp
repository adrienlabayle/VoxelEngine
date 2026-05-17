#include "ShaderStorageBuffer.h"

#include <glad/glad.h>

ShaderStorageBuffer::ShaderStorageBuffer()
{
    glGenBuffers(1, &m_RendererID);
}

ShaderStorageBuffer::~ShaderStorageBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void ShaderStorageBuffer::Bind(unsigned int bindingPoint) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID);
}

void ShaderStorageBuffer::Unbind() const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::SetData(const void* data, unsigned int size)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
}

void ShaderStorageBuffer::UpdateData(const void* data, unsigned int size, unsigned int offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

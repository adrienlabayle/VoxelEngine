#include "DrawIndirectBuffer.h"

DrawIndirectBuffer::DrawIndirectBuffer()
{
    glGenBuffers(1, &m_RendererID);
}

DrawIndirectBuffer::~DrawIndirectBuffer()
{
    glDeleteBuffers(1, &m_RendererID);
}

void DrawIndirectBuffer::Clear()
{
    m_Commands.clear();
}

void DrawIndirectBuffer::AddCommand(uint32_t faceCount, uint32_t offset, uint32_t chunkIndex)
{
    DrawCommand cmd;
    cmd.count = faceCount * 6;  // 6 vertices per face
    cmd.instanceCount = 1;
    cmd.first = offset * 6;     // offset in vertices in the mega SSBO
    cmd.baseInstance = chunkIndex;     // index of the chunk (position of the chunk)
    m_Commands.push_back(cmd);
}

void DrawIndirectBuffer::Upload()
{
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_RendererID);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, m_Commands.size() * sizeof(DrawCommand), m_Commands.data(), GL_DYNAMIC_DRAW);
}

void DrawIndirectBuffer::Bind() const
{
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_RendererID);
}
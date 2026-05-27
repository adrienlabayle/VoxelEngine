#include "MegaSSBO.h"

#include <iostream>

MegaSSBO::MegaSSBO()
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);

    // Preallocate the mega buffer (only size, no data)
    glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_FACES * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);

    // At the start all the buffer is free
    m_FreeList.push_back({ 0, MAX_FACES });
}

MegaSSBO::~MegaSSBO()
{
    glDeleteBuffers(1, &m_RendererID);
}

void MegaSSBO::Move(uint32_t srcOffset, uint32_t dstOffset, uint32_t faceCount)
{
    glBindBuffer(GL_COPY_READ_BUFFER, m_RendererID);
    glBindBuffer(GL_COPY_WRITE_BUFFER, m_RendererID);
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset * sizeof(uint32_t), dstOffset * sizeof(uint32_t), faceCount * sizeof(uint32_t)); // Source offset in byte, destination offset in byte, size in byte
}

void MegaSSBO::ResetFreeList(uint32_t usedFaces)
{
    m_FreeList.clear();
    m_FreeList.push_back({ usedFaces, MAX_FACES - usedFaces });
}

uint32_t MegaSSBO::Allocate(uint32_t faceCount)
{
    // First fit : we take the first bloc big enough
    for (auto it = m_FreeList.begin(); it != m_FreeList.end(); ++it)
    {
        if (it->size >= faceCount)
        {
            uint32_t offset = it->offset;

            if (it->size == faceCount)
            {
                // If the bloc is exactly the right size, we remove him
                m_FreeList.erase(it);
            }
            else
            {
                // If the bloc is bigger, we reduce him
                it->offset += faceCount;
                it->size -= faceCount;
            }

            return offset;
        }
    }

    std::cerr << "[MegaSSBO] Plus de place !" << std::endl;
    return UINT32_MAX; // Error
}

void MegaSSBO::Free(uint32_t offset, uint32_t faceCount)
{
    // We put back the bloc in the free list
    auto it = m_FreeList.begin();
    while (it != m_FreeList.end() && it->offset < offset)
        ++it;

    m_FreeList.insert(it, { offset, faceCount });

    Merge();
}

void MegaSSBO::Merge()
{
    // Merge the adjacents bloc
    for (auto it = m_FreeList.begin(); it != m_FreeList.end(); )
    {
        auto next = std::next(it);
        if (next == m_FreeList.end())
            break;

        if (it->offset + it->size == next->offset)
        {
            // Merge
            it->size += next->size;
            m_FreeList.erase(next);
            // We stay on it to check if the next is also adjacent
        }
        else
        {
            ++it;
        }
    }
}

void MegaSSBO::Upload(const void* data, uint32_t faceCount, uint32_t offset)
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_RendererID);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset * sizeof(uint32_t), faceCount * sizeof(uint32_t), data);
}

void MegaSSBO::Bind(unsigned int bindingPoint) const
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, m_RendererID);
}
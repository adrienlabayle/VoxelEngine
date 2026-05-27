#pragma once

#include <glad/glad.h>

#include "Vertex.h"

#include <vector>
#include <list>

class MegaSSBO
{
public:
    // Size of the mega buffer in terme of face nbr
    static constexpr uint32_t MAX_FACES = 20'000'000;

    MegaSSBO();
    ~MegaSSBO();

    void Move(uint32_t srcOffset, uint32_t dstOffset, uint32_t faceCount);
    void ResetFreeList(uint32_t usedFaces);

    // Allocate one bloc for one chunk, return the offset in terme of face nbr
    uint32_t Allocate(uint32_t faceCount);

    // Free the bloc of a chunk
    void Free(uint32_t offset, uint32_t faceCount);

    // Upload the faces of a chunk at his offset
    void Upload(const void* data, uint32_t faceCount, uint32_t offset);

    void Bind(unsigned int bindingPoint) const;

private:
    void Merge(); // Merge the adjacent free blocks

    struct FreeBlock {
        uint32_t offset;
        uint32_t size;
    };

    GLuint m_RendererID;
    std::list<FreeBlock> m_FreeList;
};
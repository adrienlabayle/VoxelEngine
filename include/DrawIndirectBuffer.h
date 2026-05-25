#pragma once
#include <glad/glad.h>

#include <vector>

struct DrawCommand
{
    uint32_t count;         // nbr of vertices (faceCount * 6), correspond to the size of the chunk in the mega SSBO
    uint32_t instanceCount; // always 1
    uint32_t first;         // offset in vertices in the mega SSBO (offset in the mega SSBO to get to the chunk)
    uint32_t baseInstance;  // index of the chunk (to get his world pos)
};

class DrawIndirectBuffer
{
public:
    DrawIndirectBuffer();
    ~DrawIndirectBuffer();

    void Clear();
    void AddCommand(uint32_t faceCount, uint32_t offset, uint32_t chunkIndex);
    void Upload();
    void Bind() const;

    inline int GetCommandCount() const { return m_Commands.size(); }
    inline const std::vector<DrawCommand>& GetCommands() const { return m_Commands; }

private:
    GLuint m_RendererID;
    std::vector<DrawCommand> m_Commands;
};
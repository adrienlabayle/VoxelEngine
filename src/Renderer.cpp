#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const ShaderStorageBuffer& ssbo, const Shader& shader, int vertexCount) const
{
    shader.Bind();
    va.Bind();
    ssbo.Bind(1);  // binding point 1

    GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexCount));
}

void Renderer::Draw(const VertexArray& va, const MegaSSBO& ssbo, const Shader& shader, const DrawIndirectBuffer& dib) const
{
    shader.Bind();
    va.Bind();
    ssbo.Bind(0);
    dib.Bind();

    //GLCall(glMultiDrawArraysIndirect(GL_TRIANGLES, nullptr, dib.GetCommandCount(), 0));
    // 
    // Test : remplace glMultiDrawArraysIndirect par une boucle manuelle
    const auto& commands = dib.GetCommands();
    for (const auto& cmd : commands)
    {
        GLCall(glDrawArraysInstancedBaseInstance(GL_TRIANGLES, cmd.first, cmd.count, cmd.instanceCount, cmd.baseInstance));
    }
}
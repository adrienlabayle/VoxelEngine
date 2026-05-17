#pragma once

class ShaderStorageBuffer
{
public:
    ShaderStorageBuffer();
    ~ShaderStorageBuffer();

    void Bind(unsigned int bindingPoint) const;
    void Unbind() const;

    void SetData(const void* data, unsigned int size);
    void UpdateData(const void* data, unsigned int size, unsigned int offset);

    inline unsigned int GetID() const { return m_RendererID; }

private:
    unsigned int m_RendererID;
};

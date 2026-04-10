#pragma once

#include <glm/glm.hpp>

class Chunk;

struct Plane {
    glm::vec3 normal;
    float distance;
};

class Frustum
{
public:
    Frustum(const glm::mat4& proj, const glm::mat4& view);
    ~Frustum();

    bool IsChunkInFrustum(float ChunkWorldX, float ChunkWorldZ);
private:
    Plane m_Planes[6]; // left, right, bottom, top, near, far
};
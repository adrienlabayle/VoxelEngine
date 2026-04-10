#include "Frustum.h"

#include "Chunk.h"

Frustum::Frustum(const glm::mat4& proj, const glm::mat4& view)
{
    glm::mat4 vp = proj * view;

    // Left
    m_Planes[0].normal.x = vp[0][3] + vp[0][0];
    m_Planes[0].normal.y = vp[1][3] + vp[1][0];
    m_Planes[0].normal.z = vp[2][3] + vp[2][0];
    m_Planes[0].distance = vp[3][3] + vp[3][0];

    // Right
    m_Planes[1].normal.x = vp[0][3] - vp[0][0];
    m_Planes[1].normal.y = vp[1][3] - vp[1][0];
    m_Planes[1].normal.z = vp[2][3] - vp[2][0];
    m_Planes[1].distance = vp[3][3] - vp[3][0];

    // Bottom
    m_Planes[2].normal.x = vp[0][3] + vp[0][1];
    m_Planes[2].normal.y = vp[1][3] + vp[1][1];
    m_Planes[2].normal.z = vp[2][3] + vp[2][1];
    m_Planes[2].distance = vp[3][3] + vp[3][1];

    // Top
    m_Planes[3].normal.x = vp[0][3] - vp[0][1];
    m_Planes[3].normal.y = vp[1][3] - vp[1][1];
    m_Planes[3].normal.z = vp[2][3] - vp[2][1];
    m_Planes[3].distance = vp[3][3] - vp[3][1];

    // Near
    m_Planes[4].normal.x = vp[0][3] + vp[0][2];
    m_Planes[4].normal.y = vp[1][3] + vp[1][2];
    m_Planes[4].normal.z = vp[2][3] + vp[2][2];
    m_Planes[4].distance = vp[3][3] + vp[3][2];

    // Far
    m_Planes[5].normal.x = vp[0][3] - vp[0][2];
    m_Planes[5].normal.y = vp[1][3] - vp[1][2];
    m_Planes[5].normal.z = vp[2][3] - vp[2][2];
    m_Planes[5].distance = vp[3][3] - vp[3][2];

    // Normalize
    for (int i = 0; i < 6; i++) {
        float len = glm::length(m_Planes[i].normal);
        m_Planes[i].normal /= len;
        m_Planes[i].distance /= len;
    }
}

Frustum::~Frustum()
{

}

bool Frustum::IsChunkInFrustum(float chunkWorldX, float chunkWorldZ)
{
    // Bounding box du chunk
    glm::vec3 min = { chunkWorldX, 0.0f, chunkWorldZ };
    glm::vec3 max = { chunkWorldX + Chunk::m_XSize, 256.0f, chunkWorldZ + Chunk::m_ZSize };

    for (int i = 0; i < 6; i++)
    {
        const Plane& plane = m_Planes[i];

        // On cherche le coin le plus "positif" dans la direction de la normale
        glm::vec3 positiveVertex = {
            plane.normal.x >= 0 ? max.x : min.x,
            plane.normal.y >= 0 ? max.y : min.y,
            plane.normal.z >= 0 ? max.z : min.z
        };

        // Si ce coin est derrière le plan, le chunk est entièrement hors frustum
        if (glm::dot(plane.normal, positiveVertex) + plane.distance < 0)
            return false;
    }

    return true;
}
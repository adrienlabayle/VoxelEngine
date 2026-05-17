#pragma once

#include "glm/glm.hpp"

struct Vertex
{
    float x, y, z;
    float u, v;
};



// Pipeline A : opaque normal blocks, packed in the SSBO
// 1 uint32 par vertex, layout :
// bits  0- 4 : x local   (0-31)
// bits  5- 9 : z local   (0-31)
// bits 10-18 : y         (0-511)
// bits 19-21 : face id   (0-5)
// bits 22-27 : texture id(0-63)
// bits 28-29 : AO level  (0-3)
// bits 30-31 : free
struct PackedVertex
{
    uint32_t data;

    static PackedVertex Pack(int x, int y, int z, int faceId, int textureId, int ao)
    {
        PackedVertex v;
        v.data = (x & 31)
            | ((z & 31) << 5)
            | ((y & 511) << 10)
            | ((faceId & 7) << 19)
            | ((textureId & 63) << 22)
            | ((ao & 3) << 28);
        return v;
    }
};
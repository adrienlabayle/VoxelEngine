#shader vertex
#version 460 core
        
// SSBO : 1 uint32 par face
layout(std430, binding = 0) readonly buffer FaceBuffer {
    uint faces[];
};

// Uniforms
uniform mat4 u_MVP;
uniform vec2 u_AtlasTileSize;  // (1.0/atlasWidth, 1.0/atlasHeight)
uniform int  u_AtlasWidth;

// Output vers le fragment shader
out vec2 v_UV;
out float v_FaceLight;  // lumière directionnelle selon la face

// Les 4 coins locaux d'une face (dans l'espace bloc 0-1) 
// et leurs UVs correspondants
// Order : bottom-left, bottom-right, top-right, top-left
const vec3 faceCorners[6][4] = vec3[][](
    // 0 : +X
    vec3[4](vec3(1,0,1), vec3(1,1,1), vec3(1,1,0), vec3(1,0,0)),
    // 1 : -X
    vec3[4](vec3(0,0,0), vec3(0,1,0), vec3(0,1,1), vec3(0,0,1)),
    // 2 : +Y (top)
    vec3[4](vec3(0,1,0), vec3(1,1,0), vec3(1,1,1), vec3(0,1,1)),
    // 3 : -Y (bottom)
    vec3[4](vec3(0,0,1), vec3(1,0,1), vec3(1,0,0), vec3(0,0,0)),
    // 4 : +Z
    vec3[4](vec3(0,0,1), vec3(0,1,1), vec3(1,1,1), vec3(1,0,1)),
    // 5 : -Z
    vec3[4](vec3(1,0,0), vec3(1,1,0), vec3(0,1,0), vec3(0,0,0))
);

// UVs locaux des 4 coins (identiques pour toutes les faces)
const vec2 cornerUVs[4] = vec2[](
    vec2(0,0), vec2(0,1), vec2(1,1), vec2(1,0)
);

// Indices pour les 2 triangles d'un quad (6 vertices → 4 coins)
const int quadIndices[6] = int[](0, 1, 2, 2, 3, 0);

// Lumière directionnelle par face (simule un soleil)
const float faceLights[6] = float[](
    0.6,   // +X
    0.6,   // -X
    1.0,   // +Y (top, plein soleil)
    0.3,   // -Y (bottom, dans l'ombre)
    0.8,   // +Z
    0.8    // -Z
);

void main()
{
    // Quel face et quel vertex dans le quad
    int faceIndex  = gl_VertexID / 6;
    int localVert  = gl_VertexID % 6;
    int cornerIndex = quadIndices[localVert];

    // Unpack du uint32
    uint data = faces[faceIndex];
    float bx = float( data        & 0x1Fu);
    float bz = float((data >>  5) & 0x1Fu);
    float by = float((data >> 10) & 0x1FFu);
    int faceId    = int((data >> 19) & 0x7u);
    int texIndex  = int((data >> 22) & 0x3Fu);

    // Position monde du vertex
    vec3 blockPos  = vec3(bx, by, bz);
    vec3 corner    = faceCorners[faceId][cornerIndex];

    // Decode chunkX and chunkZ from gl_BaseInstance
    uint base = uint(gl_BaseInstance);
    int chunkX = int(base & 0xFFFFu) - 32768;
    int chunkZ = int((base >> 16u) & 0xFFFFu) - 32768;
    vec3 chunkWorldPos = vec3(float(chunkX) * 16.0, 0.0, float(chunkZ) * 16.0);

    vec3 worldPos  = chunkWorldPos + blockPos + corner;

    gl_Position = u_MVP * vec4(worldPos, 1.0);

    // UV dans l'atlas
    float tx = float(texIndex % u_AtlasWidth);
    float ty = float(texIndex / u_AtlasWidth);
    vec2 localUV = cornerUVs[cornerIndex];
    v_UV = (vec2(tx, ty) + localUV) * u_AtlasTileSize;

    // Lumière directionnelle
    v_FaceLight = faceLights[faceId];
}



#shader fragment
#version 460 core
        
in vec2 v_UV;
in float v_FaceLight;

out vec4 fragColor;

uniform sampler2D u_Texture;

void main()
{
    vec4 texColor = texture(u_Texture, v_UV);
    fragColor = vec4(texColor.rgb * v_FaceLight, texColor.a);
}
cmake_minimum_required(VERSION 3.20)
project(V0.11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# REMARQUE : ${CMAKE_SOURCE_DIR} designe le dossier dans lequelle est present CMakeLists.txt

# Sources et headers explicitement listes
add_executable(V0.11
    src/main.cpp
    src/Renderer.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Chunk.cpp
    src/Mesh.cpp
    src/Atlas.cpp
    src/Camera.cpp
    src/World.cpp
    src/PerlinNoise.cpp
    src/Frustum.cpp
    src/TerrainGenerator.cpp
    src/VoronoiNoise.cpp
    src/TreeGenerator.cpp

    vendor/stb_image/stb_image.cpp
    vendor/stb_image/stb_image.h

    tests/Test.cpp
    tests/Test.h
    tests/TestClearColor.cpp
    tests/TestClearColor.h
    tests/TestTexture2D.cpp
    tests/TestTexture2D.h
    tests/TestCube3D.cpp
    tests/TestCube3D.h

    include/Renderer.h
    include/VertexBuffer.h
    include/IndexBuffer.h
    include/VertexArray.h
    include/VertexBufferLayout.h
    include/Shader.h
    include/Texture.h
    include/Vertex.h
    include/Chunk.h
    include/Mesh.h
    include/Atlas.h
    include/Camera.h
    include/World.h
    include/PerlinNoise.h
    include/Frustum.h
    include/ThreadSafeQueue.h
    include/ThreadSafeSet.h
    include/TerrainGenerator.h
    include/VoronoiNoise.h
    include/TreeGenerator.h
)

file(GLOB TESTS_SOURCES tests/*.cpp)
target_sources(V0.11 PRIVATE ${TESTS_SOURCES})

file(GLOB IMGUI_SOURCES vendor/imgui/*.cpp)
target_sources(V0.11 PRIVATE ${IMGUI_SOURCES})

file(GLOB_RECURSE SHADER_FILES "${PROJECT_SOURCE_DIR}/res/*.shader")
target_sources(V0.11 PRIVATE ${SHADER_FILES})

# Headers (et autres dossiers que src)
target_include_directories(V0.11 PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/vendor
    ${PROJECT_SOURCE_DIR}/tests
)

# Explique a VS comment organniser tt les fichier visuellement
source_group(TREE ${CMAKE_SOURCE_DIR} FILES
    src/main.cpp
    src/Renderer.cpp
    src/VertexBuffer.cpp
    src/IndexBuffer.cpp
    src/VertexArray.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/Chunk.cpp
    src/Mesh.cpp
    src/Atlas.cpp
    src/Camera.cpp
    src/World.cpp
    src/PerlinNoise.cpp
    src/Frustum.cpp
    src/TerrainGenerator.cpp
    src/VoronoiNoise.cpp
    src/TreeGenerator.cpp

    vendor/stb_image/stb_image.cpp
    vendor/stb_image/stb_image.h

    tests/Test.cpp
    tests/Test.h
    tests/TestClearColor.cpp
    tests/TestClearColor.h
    tests/TestTexture2D.cpp
    tests/TestTexture2D.h
    tests/TestCube3D.cpp
    tests/TestCube3D.h

    include/Renderer.h
    include/VertexBuffer.h
    include/IndexBuffer.h
    include/VertexArray.h
    include/VertexBufferLayout.h
    include/Shader.h
    include/Texture.h
    include/Vertex.h
    include/Chunk.h
    include/Mesh.h
    include/Atlas.h
    include/Camera.h
    include/World.h
    include/PerlinNoise.h
    include/Frustum.h
    include/ThreadSafeQueue.h
    include/ThreadSafeSet.h
    include/TerrainGenerator.h
    include/VoronoiNoise.h
    include/TreeGenerator.h

    ${SHADER_FILES}
)

# Working directory pour VS
set_target_properties(V0.11 PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Librairies
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

target_link_libraries(V0.11 PRIVATE glfw glad::glad OpenGL::GL)

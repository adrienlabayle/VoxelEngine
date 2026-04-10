# Voxel Engine (C++ / OpenGL)

## Overview

This project is a C++ voxel engine (Minecraft-like) developed as a personal project to learn game engine programming and real-time rendering.

I am currently a 4th-year engineering student at Polytech Clermont-Ferrand (France), specializing in Applied Mathematics and Data Science. While my curriculum is not purely computer science-oriented, I have studied C, C++, Python, algorithms, and databases (SQL).

This project reflects my goal of becoming a C++ game engine developer.

The engine is built using OpenGL, with ImGui used for debugging and development interface.

---

## Project Goals

- Learn modern OpenGL and real-time rendering
- Understand voxel engine architecture
- Work with performance-critical systems (CPU/GPU)
- Practice multithreading and memory management

---

## Features

- Voxel rendering (OpenGL)
- Chunk-based world system
- Separate mesh per chunk (opaque / transparent)
- Face culling & blending
- Texture atlas
- Frustum culling
- Multithreaded chunk generation
- Basic procedural terrain (Perlin noise)

---

## Architecture

- **World system** managing chunk loading/unloading based on render distance
- Each chunk has its own:
  - VAO / VBO / IBO
  - Separate meshes for opaque and transparent blocks
- Dynamic remeshing of neighboring chunks
- CPU-side mesh generation

---

## Progression

- **v0.11** → Current version
  - Voxel rendering
  - Chunk system
  - Face culling & culled mesh
  - Texture atlas
  - Frustum culling (+ performance gain: ~20 → 40 FPS)
  - Limited mesh generation per frame (+ stability improvement)
  - Multithreading (+ performance gain: ~75 FPS average)

---

## Performance Notes

- Initial performance: ~20 FPS while moving  
- After frustum culling: ~40 FPS  
- After multithreading: ~75 FPS  

Main bottlenecks identified:
- Chunk generation cost
- Mesh generation spikes
- Lack of LOD and lighting optimizations

---

## Current Limitations

- No lighting system yet
- Greedy meshing not fully optimized
- Basic terrain generation
- GPU data could be further optimized

---

## Future Work

- Improved procedural generation (advanced noise)
- Multiple biome/chunk types
- Tree generation
- Lighting system
- Level of Detail (LOD)
- Vertex pulling
- GPU data optimization

---

## Background

This project started after following The Cherno's OpenGL series, and continues beyond it as a way to explore more advanced rendering and engine concepts.
# Voxel Engine (C++ / OpenGL)

## Overview

This is a C++ voxel engine (Minecraft-like) developed as a personal project to learn real-time rendering, engine architecture, and low-level performance optimization.

The project is written in modern C++ with OpenGL as the rendering API. It includes ImGui for debugging and development tools.

It is an ongoing long-term project aimed at exploring game engine design, procedural generation, and GPU/CPU optimization techniques.

---

## Background

This project started after following The Cherno OpenGL series:
https://www.youtube.com/watch?v=W3gAzLwfIP0&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2

It initially began as a learning project for modern OpenGL, but progressively evolved into a full voxel engine with:

- custom chunk system
- multithreaded mesh generation
- procedural terrain generation
- GPU performance optimization experiments

---

## Tech Stack

- C++ (modern C++17/20)
- OpenGL
- GLSL shaders
- ImGui (debug UI)
- GLM (math)

---

## Features

### World System
- Chunk-based world streaming around player
- Dynamic chunk loading/unloading based on render distance
- CPU voxel storage per chunk

### Rendering
- Separate pipelines:
  - opaque geometry
  - transparent geometry
- Texture atlas system
- Frustum culling

### Voxel System
- Full 3D voxel grid per chunk
- Face culling (only visible faces are generated)
- Neighbor-aware sampling for seamless chunk borders

### Multithreading
- Worker thread pool for chunk meshing
- Thread-safe job / result queues
- Asynchronous mesh generation pipeline

### Procedural Generation
- Perlin noise terrain generation
- Early biome experimentation (in progress)

---

## Architecture

### Main thread
- Chunk creation / deletion based on camera position
- Rendering (OpenGL draw calls)
- Applies generated meshes to GPU buffers

### Worker threads
- Generate chunk meshes asynchronously
- Convert voxel data into vertex/index buffers

### Chunk rendering
Each chunk contains:
- CPU voxel data
- GPU buffers:
  - VAO / VBO / IBO for opaque geometry
  - VAO / VBO / IBO for transparent geometry

This results in:
- 2 draw calls per chunk

---

## Progression

- **v0.11 → First Git-tracked version of the engine**
  - Initial voxel world implementation
  - Chunk system with streaming around player
  - Face culling (only visible faces generated)
  - Separate opaque / transparent rendering pipeline
  - Texture atlas integration
  - Frustum culling (significant reduction of rendered chunks)
  - Mesh generation throttling per frame (stability improvement)
  - Multithreaded mesh generation system
  - Thread-safe job/result queues
  - Performance improvement: ~20 FPS → ~70–90 FPS stable

> FPS measurements are done with a render distance of 20 chunks  
> (world area ≈ (1 + 20×2)² chunks around the player)

---

## Performance Notes

### Before optimizations
- ~20 FPS during chunk updates (severe CPU spikes)

### After frustum culling
- ~40 FPS (reduced rendered workload)

### After multithreading
- ~70–90 FPS stable depending on scene

---

## Current Bottlenecks

The CPU is no longer the main limitation.

Current bottleneck is GPU-side:

- high vertex density per chunk
- large number of draw calls (per-chunk rendering)
- no LOD system
- limited GPU batching/instancing

---

## Previous CPU bottlenecks (now solved)

- Chunk generation spikes when moving fast
- Mesh rebuild stuttering
- Heavy synchronous mesh building

Solved via:
- worker thread pool
- job queue system
- per-frame remeshing limits

---

## Current Limitations

- No lighting system yet (sunlight / torchlight)
- No LOD system
- Greedy meshing tested but not retained (complexity vs gain not worth it in current pipeline)
- Basic procedural generation
- GPU data layout still not optimized

---

## Future Work

- Biome system (plains / desert / snow with smooth transitions)
- Vegetation system (trees, foliage)
- Lighting system (block light + sunlight propagation)
- Level of Detail (LOD system for chunks)
- GPU optimization:
  - reduced vertex density
  - potential vertex pulling / GPU-driven rendering
- Improved procedural generation (multi-noise terrain system)

---

## Design Notes

- Greedy meshing (including binary variants) was experimented with but did not provide sufficient benefit due to:
  - increased complexity
  - interaction with future lighting systems
  - GPU-side bottlenecks dominating performance

- Frustum culling provided a strong performance improvement with minimal architectural cost

- Current development focus has shifted from CPU optimization → GPU workload reduction
A highly optimized, hardware-accelerated 2D particle simulation engine built from scratch in C++17 and OpenGL. 

The primary goal of this project is to explore low-level graphics API architecture, Data-Oriented Design (DOD), and rendering optimization techniques. The engine currently utilizes Instanced Rendering to drastically reduce CPU-GPU bottlenecking, allowing the rendering of thousands of entities in a single draw call.

## Tech Stack
* **Language:** C++ (Modern standards)
* **Graphics API:** OpenGL 3.3+ Core Profile
* **Libraries:** GLFW (Windowing), GLAD (Function loading)
* **Architecture:** Custom Shader Pipeline, Instanced Rendering, DOD-focused Particle Structures.

## 🎯 Project Roadmap & Milestones

This project is structured around incremental performance and physics complexity milestones.

- [x] **Phase 1: Foundation & Pipeline**
  - Window creation and OpenGL context setup.
  - Custom Shader Program class with RAII resource management.
  - Streaming VBOs for dynamic position and color updates.
  - Instanced Rendering via `glDrawArraysInstanced`.

- [ ] **Phase 2: Physics & Interactivity**
  - Implement Delta Time loop for framerate-independent physics.
  - Add basic collision detection (AABB or screen boundaries).
  - Mouse interaction (repulsion/attraction forces).

- [ ] **Phase 3: The N-Body Problem**
  - Implement gravitational attraction between all particles.
  - Color mapping based on particle velocity/mass.

- [ ] **Phase 4: The 1-Million Particle Challenge (Performance)**
  - Migrate calculations from raw loops to CPU Multithreading (Thread pools).
  - Implement Spatial Partitioning (Grid/Quadtree) to optimize $O(N^2)$ complexity.
  - *Stretch Goal:* Port the physics integration step to the GPU using Compute Shaders.
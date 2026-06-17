# 2D Particle Simulation Engine

A highly optimized, hardware-accelerated 2D particle simulation engine built from scratch in C++17 and OpenGL.

The primary goal of this project is to explore low-level graphics API architecture, data locality, and multi-threaded CPU physics optimization. The engine implements spatial partitioning and task-based parallelism to benchmark the limits of modern CPU architectures like the Ryzen 7 5700X.

## Tech Stack
* **Language:** C++17
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Libraries:** GLFW (Windowing & Input), GLAD (Extension Loader), Dear ImGui (UI Metrics)
* **Optimization Tech:** Spatial Partitioning (Uniform Grid), Task-Based Multithreading (`std::async`), Instanced Rendering.

## 🚀 Architecture & Optimization Highlights

* **Spatial Partitioning (Uniform Grid):** Reduced the naive collision detection complexity from $O(N^2)$ to an average of $O(N)$ by mapping particles to a dynamic uniform grid using cache-friendly intrusive linked lists (`head` and `next` arrays).
* **Task-Based Parallelism:** Implemented a two-phase lock-free parallel execution pipeline using `std::async`. 
  * *Phase 1:* Parallel integration (kinematics, wall collisions, and mouse forces) over contiguous memory blocks.
  * *Phase 2 (Franjas con Zona de Amortiguación):* Parallel grid-column collision sorting, leaving the intersection boundaries (costuras) to be resolved sequentially by the main thread to eliminate race conditions without mutex overhead.
* **Instanced Rendering:** Drastically reduced CPU-to-GPU overhead by packing dynamic positions, sizes, and colors into streaming Vertex Buffer Objects (VBOs) and rendering 100,000 entities in a single `glDrawArraysInstanced` call.

## 📊 Performance Benchmarks (Ryzen 7 5700X / 100k Particles)
* **Pure Single-Threaded $O(N^2)$ Baseline:** Stack overflow limits / Unplayable slide-show.
* **Uniform Grid (Single-Threaded):** ~15 FPS.
* **Uniform Grid + Multi-threaded Pipeline (8-16 Tasks):** **~30 FPS stable** under homogeneous distribution.
* *Note on Clumping:* High particle density clusters (e.g., pulling all 100k particles into a single grid sector using mouse attraction) temporarily degrades performance back to local $O(N^2)$ due to spatial grid saturation.

## 🎯 Project Roadmap & Milestones

- [x] **Phase 1: Foundation & Pipeline**
  - Window creation and OpenGL context setup.
  - Custom Shader Program class with RAII resource management.
  - Streaming VBOs for dynamic position and color updates.
  - Instanced Rendering via `glDrawArraysInstanced`.

- [x] **Phase 2: Physics, Spatial Grid & Interactivity**
  - Implement Delta Time loop for framerate-independent physics.
  - Screen boundary and elastic particle-to-particle collision response.
  - Real-time mouse interaction (repulsion/attraction forces).
  - Spatial Partitioning Grid setup with $O(N)$ mapping.

- [x] **Phase 3: The Multi-Threaded CPU Challenge**
  - Migrated calculations to CPU Multithreading via task dispatching (`std::async`).
  - Implemented lock-free column-based slicing for safe parallel physical simulation.
  - Profiled thread overhead and hardware-induced bottlenecks (AoS vs. SoA, cache locality, and context switching).
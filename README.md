



# 2D Particle Simulation Engine

https://github.com/user-attachments/assets/5e317445-0013-4c61-85df-37f44c213f54


*A real-time simulation of 10,000 particles interacting with screen boundaries and mouse forces, color-mapped via normalized UV coordinates to visualize flow.*

A highly optimized, hardware-accelerated 2D particle simulation engine built from scratch in C++17 and OpenGL.

The primary goal of this project is to explore low-level graphics API architecture, data locality, and multi-threaded CPU physics optimization. The engine implements spatial partitioning and a custom persistent thread pool architecture to benchmark the limits of modern CPU architectures like the Ryzen 7 5700X.

## Tech Stack
* **Language:** C++17
* **Graphics API:** OpenGL 3.3 (Core Profile)
* **Libraries:** GLFW (Windowing & Input), GLAD (Extension Loader), Dear ImGui (UI Metrics)
* **Optimization Tech:** Spatial Partitioning (Uniform Grid), Persistent Thread Pool (Manual Synchronization), Instanced Rendering.

## Architecture & Optimization Highlights

* **Spatial Partitioning (Uniform Grid):** Reduced the naive collision detection complexity from $O(N^2)$ to an average of $O(N)$ by mapping particles to a dynamic uniform grid using cache-friendly intrusive linked lists (`head` and `next` arrays).
* **Persistent Thread Pool & Synchronization:** Evolved from standard `std::async` dispatching to a custom `ThreadPool` to eliminate task-allocation overhead and runtime stuttering.
  * *Lock-Free Execution:* 16 persistent worker threads process physics batches concurrently without locking.
  * *Last Thread Standing Barrier:* Sychronization is handled via an atomic countdown and a `std::condition_variable`. The main thread sleeps at 0% CPU cost until the exact microsecond the final worker completes the frame, ensuring safe execution before moving to the sequential grid-mapping phase.
  * *Column-based Slicing:* Parallel grid-column collision sorting leaves the intersection boundaries (buffer zones) to be resolved safely by the main thread.
* **Instanced Rendering:** Drastically reduced CPU-to-GPU overhead by packing dynamic positions, sizes, and colors into streaming Vertex Buffer Objects (VBOs) and rendering 100,000 entities in a single `glDrawArraysInstanced` call.
* **Zero-Cost Visual Debugging:** Particle colors are initialized using normalized spatial coordinates (UV gradient mapping) to visualize fluid-like motion patterns without adding per-frame CPU overhead.

## Performance Benchmarks (Ryzen 7 5700X)

https://github.com/user-attachments/assets/b2139b82-07b6-4e46-90b6-f9a1f2e73421

*Stress test: 100,000 active particles rendering at stable framerates.*

* **Pure Single-Threaded $O(N^2)$ Baseline:** Stack overflow limits / Unplayable slide-show.
* **Uniform Grid (Single-Threaded):** ~15 FPS.
* **Uniform Grid + `std::async` Pipeline:** ~30 FPS (Suffered from micro-stuttering due to thread creation/destruction overhead).
* **Uniform Grid + Persistent Thread Pool (16 Workers):** **~40 FPS stable** under homogeneous distribution.
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
  - Initial migration to CPU Multithreading via `std::async`.
  - Implemented lock-free column-based slicing for safe parallel physical simulation.
  - **Refactor:** Replaced `std::async` with a custom Persistent Thread Pool to fix task allocation stuttering.
  - Implemented explicit thread synchronization using `std::unique_lock`, `std::condition_variable`, and atomics.

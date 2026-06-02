cmake_minimum_required(VERSION 3.15)
project(ParticleSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include(FetchContent)


FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


FetchContent_MakeAvailable(glfw)


include_directories(include)


add_executable(ParticleSimulation main.cpp src/glad.c ShaderProgram.cpp)


target_link_libraries(ParticleSimulation PRIVATE glfw)
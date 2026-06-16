#pragma once
#include <array>

struct Particle
{
    std::array<float, 3> position;
    std::array<float, 3> velocity;
    std::array<unsigned char, 4> color;
    float size;
    float life;
};
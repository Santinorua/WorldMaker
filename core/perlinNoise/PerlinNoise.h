#pragma once
#include <vector>
#include "Vec2.h"
#include "Vec4.h"
#include "PRNG.h"
#include "Interpolations.h"
#include "cmath"
#include <array>

namespace WorldMaker {

    class PerlinNoise {
        uint64_t m_seed;
        double m_scale;
        double m_amplitude;
        int m_height;
        int m_width;

        double dotGradient(int x, int y, double sampleX, double sampleY);

    public:
        PerlinNoise(int width, int height, double frequency, uint64_t seed, double amplitude = 1.0);
        double getPerlinNoise(int x, int y);
    };


}

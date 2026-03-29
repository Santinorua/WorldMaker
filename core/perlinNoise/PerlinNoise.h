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
        double m_scale;
        uint64_t m_seed;
        int m_height;
        int m_width;

        const Vec2 TopLeft = Vec2(0.5, -0.5);
        const Vec2 TopRight = Vec2(-0.5, -0.5);
        const Vec2 BottomLeft = Vec2(0.5, 0.5);
        const Vec2 BottomRight = Vec2(-0.5, 0.5);
        public:
        double dotGradient(int x, int y, double sampleX, double sampleY);
        PerlinNoise(int width, int height, double frequency, uint64_t seed);
        double getPerlinNoise(int x, int y);
    };
}

#pragma once

#include <cstdint>
#include "PerlinNoise.h"

namespace WorldMaker {
    class WorldGenerator {
    public:
        uint64_t m_seed;
        double m_yScale;
        FractalNoise m_erosion;
        FractalNoise m_continentalness;
        FractalNoise m_temperature;
        FractalNoise m_humidity;
        FractalNoise m_base;
    public:
        double getHeight(float x, float z);
        WorldGenerator(double yScale, uint64_t seed);
    };
}

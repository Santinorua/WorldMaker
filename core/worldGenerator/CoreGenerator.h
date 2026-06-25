#pragma once

#include <cstdint>
#include "PerlinNoise.h"
#include "Vertex.h"

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
        double getHeight(double erosion, double continentalness, double base);
        Vertex getVertex(float x, float z);
        WorldGenerator(double yScale, uint64_t seed);
    };
}

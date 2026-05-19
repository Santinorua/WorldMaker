#pragma once
#include <cstdint>
#include "PerlinNoise.h"

namespace WorldMaker {
    class BiomeGenerator {
    public:
        uint64_t m_seed;
        FractalNoise m_erosion;
        FractalNoise m_continentalness;
        FractalNoise m_temperature;
        FractalNoise m_humidity;
    public:
        BiomeGenerator(int width, int height, uint64_t seed);
    };
}

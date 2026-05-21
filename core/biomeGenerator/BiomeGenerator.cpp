#include "BiomeGenerator.h"
#include "PRNG.h"

namespace WorldMaker {

    BiomeGenerator::BiomeGenerator(int width, int height, uint64_t seed) {
        m_continentalness = FractalNoise(width, height, 4.0, 1.0, seed, 3, 1.66, 0.33);
        uint64_t nextSeed = PRNG::nextNumber64(seed);
        m_erosion = FractalNoise(width, height, 7.0, 1.0, nextSeed, 4, 2.0, 0.4);
        nextSeed = PRNG::nextNumber64(seed);
        m_temperature = FractalNoise(width, height, 2.0, 1.0, nextSeed, 3, 3, 0.3);
        nextSeed = PRNG::nextNumber64(seed);
        m_humidity = FractalNoise(width, height, 2.0, 1.0, nextSeed, 4, 2.5, 0.25);
    }


}
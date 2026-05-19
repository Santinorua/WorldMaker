#include "BiomeGenerator.h"
#include "PRNG.h"

namespace WorldMaker {

    BiomeGenerator::BiomeGenerator(int width, int height, uint64_t seed) {
        m_continentalness = FractalNoise(width, height, 4.0, 1.0*2, seed, 3, 2.0, 0.33);
        uint64_t nextSeed = PRNG::nextNumber64(seed) +1;
        m_erosion = FractalNoise(width, height, 7.0, 1.0*2, nextSeed, 4, 2.0, 0.6);
    }


}
#include "CoreGenerator.h"
#include "PRNG.h"

namespace WorldMaker {

    WorldGenerator::WorldGenerator(int width, int height, uint64_t seed) {
        m_seed = seed;
        m_continentalness = FractalNoise(width, height, 40.0, 1.0, m_seed, 3, 1.66, 0.33);
        uint64_t nextSeed = PRNG::nextNumber64(m_seed);
        m_erosion = FractalNoise(width, height, 70.0, 1.0, nextSeed, 4, 2.0, 0.4);
        nextSeed = PRNG::nextNumber64(seed);
        m_temperature = FractalNoise(width, height, 20.0, 1.0, nextSeed, 3, 3, 0.3);
        nextSeed = PRNG::nextNumber64(seed);
        m_humidity = FractalNoise(width, height, 20.0, 1.0, nextSeed, 4, 2.5, 0.25);
    }


}
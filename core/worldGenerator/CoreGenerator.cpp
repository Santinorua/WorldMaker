#include "CoreGenerator.h"

#include "DebugUtils.h"
#include "PRNG.h"
#include "Interpolations.h"

namespace WorldMaker {

    WorldGenerator::WorldGenerator(double yScale, uint64_t seed) {
        m_seed = seed;
        m_yScale = yScale;
        m_continentalness = FractalNoise(40.0, 1.0, m_seed, 3, 1.66, 0.33);
        uint64_t nextSeed = PRNG::nextNumber64(m_seed);
        m_erosion = FractalNoise(70.0, 1.0, nextSeed, 4, 2.0, 0.4);
        nextSeed = PRNG::nextNumber64(nextSeed);
        m_temperature = FractalNoise(20.0, 1.0, nextSeed, 3, 3, 0.3);
        nextSeed = PRNG::nextNumber64(nextSeed);
        m_humidity = FractalNoise(20.0, 1.0, nextSeed, 4, 2.5, 0.25);
        nextSeed = PRNG::nextNumber64(nextSeed);
        m_base = FractalNoise(60.0, 1.5, nextSeed, 4, 2.0, 0.5);
    }

    double WorldGenerator::getHeight(float x, float z) {
        double continentalness = m_continentalness.getNoise(x, z);
        double erosion = (m_erosion.getNoise(x, z) + 0.5) * 0.5;
        double base = m_base.getNoise(x, z);
        double continentalnessModifier;

        if (continentalness >= -0.1) {
            continentalnessModifier = 0;

        } else if (continentalness >= -0.6) {
            continentalnessModifier = Lerp(-0.8, 0.0, ((continentalness+0.6) * 2), true);
        } else {
            continentalnessModifier = -0.8;
        }

        double final = base * erosion + continentalnessModifier;

        return final * m_yScale;
    }


}
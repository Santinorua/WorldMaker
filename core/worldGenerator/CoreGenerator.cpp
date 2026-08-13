#include "CoreGenerator.h"

#include "DebugUtils.h"
#include "PRNG.h"
#include "Interpolations.h"
#include "BiomeGenerator.h"

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

    double WorldGenerator::getHeight(double erosion, double continentalness, double base) {
        double continentalnessModifier;

        if (continentalness >= -0.1) {
            continentalnessModifier = 0;

        } else if (continentalness >= -0.6) {
            continentalnessModifier = Lerp(-0.5, 0.0, ((continentalness+0.6) * 2), true);
        } else {
            continentalnessModifier = -0.5;
        }

        double erosionModifier;
        if (erosion >= 0.8) {
            erosionModifier = erosion;
        } else if (erosion >= 0.6) {
            erosionModifier = Lerp(0.2, 0.8, (erosion - 0.6) * 5, false);
        } else {
            erosionModifier = Lerp(0.0, 0.2, (erosion * 10.0) / 6.0, false);
        }

        double final = base * erosionModifier + continentalnessModifier;

        return final;
    }

    Vertex WorldGenerator::getVertex(float x, float z) {
        double continentalness = m_continentalness.getNoise(x, z);
        double erosion = (m_erosion.getNoise(x, z) + 0.5);
        double temperature = m_temperature.getNoise(x, z);
        double humidity = m_humidity.getNoise(x, z);
        double base = m_base.getNoise(x, z);

        double height = getHeight(erosion, continentalness, base);

        double params[4] = {erosion, continentalness, temperature, humidity};
        Biome biome = BiomeGenerator::getBiome(params);


        Vertex v;
        v.m_color = {1.0,1.0,1.0,1.0};

        if (continentalness >= -0.1) {
            v.m_color = biome.biomeColor;
        } else if (continentalness >= -0.6) {
            v.m_color = {Lerp(0.0, biome.biomeColor.x, ((continentalness+0.6) * 2), true), Lerp(0.0, biome.biomeColor.y, ((continentalness+0.6) * 2), true), Lerp(1.0, biome.biomeColor.z, ((continentalness+0.6) * 2), true), 1.0};
        } else {
            v.m_color = {0.0, 0.0, 1.0, 1.0};
        }
        
        v.m_uv = { static_cast<float>(x) / 10.0f, static_cast<float>(z) / 10.0f };
        v.m_position = { static_cast<float>(x), static_cast<float>(height * m_yScale), static_cast<float>(z) };

        return v;
    }


}
#include "PerlinNoise.h"

#include "../rendering/DebugUtils.h"


namespace WorldMaker {

    double PerlinNoise::dotGradient(int x, int y, double sampleX, double sampleY) {
        // Get the random gradient vector for the grid point
        Vec2 gradient = PRNG::randomVector2(x, y, m_seed);

        // Calculate the distance vector from the grid point to the sample point
        double dx = sampleX - x;
        double dy = sampleY - y;

        // Return the dot product
        return dx * gradient.x + dy * gradient.y;
    }

    double PerlinNoise::getPerlinNoise(int x, int y) {
        // Calculate the position of the point asked for in the noise space
        double sampleX = x / m_scale;
        double sampleY = y / m_scale;

        // Determine the grid cell coordinates surrounding the point
        int fX = (int)sampleX;
        int fY = (int)sampleY;

        // Calculate the dot product between the random gradients and the distance vectors for each of the four corners
        double gradients[4] = {
            dotGradient(fX, fY, sampleX, sampleY),
            dotGradient(fX + 1, fY, sampleX, sampleY),
            dotGradient(fX, fY + 1, sampleX, sampleY),
            dotGradient(fX + 1, fY + 1, sampleX, sampleY)
        };


        // Perform bilinear interpolation to get the final noise value
        return Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]), sampleX - std::floor(sampleX), sampleY - std::floor(sampleY));
    }

    PerlinNoise::PerlinNoise(int width, int height, double frequency, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = width / frequency;
        m_seed = seed;
    }


    double PerlinNoise3D::dotGradient3D(int x, int y, int z, double sampleX, double sampleY, double sampleZ) {
        // Get the random gradient vector for the grid point
        Vec3 gradient = PRNG::randomVector3(x, y, z, m_seed);

        // Calculate the distance vector from the grid point to the sample point
        double dx = sampleX - x;
        double dy = sampleY - y;
        double dz = sampleZ - z;

        // Return the dot product
        return dx * gradient.x + dy * gradient.y + dz * gradient.z;
    }

    double PerlinNoise3D::getPerlinNoise3D(int x, int y, int z) {
        // Calculate the position of the point asked for in the noise space
        double sampleX = x / m_scale;
        double sampleY = y / m_scale;
        double sampleZ = z / m_heightScale;

        // Determine the grid cell coordinates surrounding the point
        int fX = (int)sampleX;
        int fY = (int)sampleY;
        int fZ = (int)sampleZ;

        // Calculate the dot product between the random gradients and the distance vectors for each of the four corners
        double gradients[8] = {
            dotGradient3D(fX, fY, fZ, sampleX, sampleY, sampleZ),
            dotGradient3D(fX + 1, fY, fZ, sampleX, sampleY, sampleZ),
            dotGradient3D(fX, fY + 1, fZ, sampleX, sampleY, sampleZ),
            dotGradient3D(fX + 1, fY + 1, fZ, sampleX, sampleY, sampleZ),
            dotGradient3D(fX, fY, fZ + 1, sampleX, sampleY, sampleZ),
            dotGradient3D(fX + 1, fY, fZ + 1, sampleX, sampleY, sampleZ),
            dotGradient3D(fX, fY + 1, fZ + 1, sampleX, sampleY, sampleZ),
            dotGradient3D(fX + 1, fY + 1, fZ + 1, sampleX, sampleY, sampleZ)
        };


        // Perform bilinear interpolation to get the final noise value
        return Trilinear(gradients, sampleX - fX, sampleY - fY, sampleZ - fZ);
    }

    PerlinNoise3D::PerlinNoise3D(int width, int height, double frequency, double heightScale, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = width / frequency;
        m_heightScale = heightScale;
        m_seed = seed;
    }


    FractalNoise::FractalNoise(int width, int height, double frequency, double amplitude, uint64_t seed, int numOctaves, double lacunarity, double persistence) {
        m_persistance = persistence;
        m_amplitude = amplitude;
        m_octaves.reserve(numOctaves);
        uint64_t currentSeed = seed;
        for (int i = 0; i < numOctaves; i++) {
            m_octaves.emplace_back(width, height, frequency, currentSeed);
            frequency *= lacunarity;
            currentSeed = PRNG::nextNumber64(currentSeed);
        }
    }

    double FractalNoise::getNoise(int x, int y) {
        double result = 0.0;
        double amplitude = m_amplitude;
        for (PerlinNoise& octave : m_octaves) {
            result += octave.getPerlinNoise(x, y) * amplitude;
            amplitude *= m_persistance;
        }
        return result;
    }

    ComplexNoise::ComplexNoise(int width, int height, int first, const std::vector<double>& amplitudes, uint64_t seed) {
            assert(first < 0);
            assert(!amplitudes.empty());
            m_amplitudes = amplitudes;
            m_octaves.reserve(amplitudes.size() * 2);
            uint64_t currentSeed = seed;
            for (int i = 0; i < amplitudes.size(); i++) {
                m_octaves.emplace_back(width, height, std::pow(2.0, (i - first)), currentSeed);
                currentSeed = PRNG::nextNumber64(seed);
                m_octaves.emplace_back(width, height, std::pow(2.0, (i - first)), currentSeed);
                currentSeed = PRNG::nextNumber64(seed);
            }
    }

    double ComplexNoise::getNoise(int x, int y) {
        double result = 0.0;
        for (int i = 0; i < m_octaves.size(); i = i+2) {
            double h = m_octaves[i].getPerlinNoise(x, y) * m_amplitudes[i/2];
            double l = m_octaves[i+1].getPerlinNoise(x, y) * m_amplitudes[i/2];
            double combined = (h + l) * 0.5;
            // result += combined * std::pow(2.0, -(i/2) - 1) / (std::pow(2.0, m_amplitudes.size()) - 1);
            result += combined;
        }
        return result;
        // double totalAmplitude = m_amplitudes.size()
    }



}

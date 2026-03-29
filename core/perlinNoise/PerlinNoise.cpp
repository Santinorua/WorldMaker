#include "PerlinNoise.h"



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
        double result = Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]), sampleX - std::floor(sampleX), sampleY - std::floor(sampleY));

        // Scale the result to the range [0, amplitude]
        return result * (m_amplitude / 2) + (m_amplitude / 2);
    }
    PerlinNoise::PerlinNoise(int width, int height, double frequency, uint64_t seed, double amplitude) {
        m_height = height;
        m_width = width;
        m_scale = width / frequency;
        m_seed = seed;
        m_amplitude = amplitude;
    }


}

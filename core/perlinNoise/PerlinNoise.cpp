#include "PerlinNoise.h"



namespace WorldMaker {

    double PerlinNoise::dotGradient(int x, int y, double sampleX, double sampleY) {
        Vec2 gradient = PRNG::randomVector2(x, y, m_seed);
        double dx = sampleX - x;
        double dy = sampleY - y;
        return (dx * gradient.x + dy * gradient.y);
    }

    double PerlinNoise::getPerlinNoise(int x, int y) {
        double sampleX = x / m_scale;
        double sampleY = y / m_scale;
        std::array<double, 4> gradients = {
            dotGradient(std::floor(sampleX), std::floor(sampleY), sampleX, sampleY),
            dotGradient(std::floor(sampleX) + 1, std::floor(sampleY), sampleX, sampleY),
            dotGradient(std::floor(sampleX), std::floor(sampleY) + 1, sampleX, sampleY),
            dotGradient(std::floor(sampleX) + 1, std::floor(sampleY) + 1, sampleX, sampleY)
        };
        return Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]), sampleX - std::floor(sampleX), sampleY - std::floor(sampleY)) * 0.5 + 0.5;
    }
    PerlinNoise::PerlinNoise(int width, int height, double frequency, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = width / frequency;
        m_seed = seed;
    }
}

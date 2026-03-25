#include "PerlinNoise.h"



namespace WorldMaker {
    std::array<Vec2,4> PerlinNoise::getVectors(double x, double y) {
        std::array<Vec2,4> result;
        result[0] = PRNG::randomVector2(std::floor(x), std::floor(y), m_seed).Normalized();
        result[1] = PRNG::randomVector2(std::ceil(x), std::floor(y), m_seed).Normalized();
        result[2] = PRNG::randomVector2(std::floor(x), std::ceil(y), m_seed).Normalized();
        result[3] = PRNG::randomVector2(std::ceil(x), std::ceil(y), m_seed).Normalized();
        return result;
    }
    std::array<double,4> PerlinNoise::getGradients(std::array<Vec2, 4> &vectors, double x, double y) {
        std::array<double,4> gradients;
        Vec2 pos = Vec2(x, y);
        for (int i = 0; i < 4; i++) {
            Vec2 dir = pos - vectors[i];
            gradients[i] = dir.DotProduct(vectors[i]);
        }
        return gradients;
    }

    double PerlinNoise::getPerlinNoise(int x, int y) {
        double sampleX = x / m_scale;
        double sampleY = y / m_scale;

        std::array<Vec2,4> vectors = getVectors(sampleX, sampleY);
        std::array<double,4> gradients = getGradients(vectors, sampleX, sampleY);

        return Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]));
    }
    PerlinNoise::PerlinNoise(int width, int height, double scale, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = scale;
        m_seed = seed;
    }
}

#include "PerlinNoise.h"



namespace WorldMaker {
    std::array<Vec2,4> PerlinNoise::getVectors(double x, double y) {
        std::array<Vec2,4> result;
        double ceilX = std::floor(x) == x ? std::floor(x) + 1 : std::ceil(x);
        double ceilY = std::floor(y) == y ? std::floor(y) + 1 : std::ceil(y);
        result[0] = PRNG::randomVector2(std::floor(x), std::floor(y), m_seed);
        result[1] = PRNG::randomVector2(ceilX, std::floor(y), m_seed);
        result[2] = PRNG::randomVector2(std::floor(x), ceilY, m_seed);
        result[3] = PRNG::randomVector2(ceilX, ceilY, m_seed);
        // std::cout << "Vectors: " << result[0].x << "," << result[0].y << " | " << result[1].x << "," << result[1].y << " | " << result[2].x << "," << result[2].y << " | " << result[3].x << "," << result[3].y << std::endl;
        return result;
    }
    std::array<double,4> PerlinNoise::getGradients(std::array<Vec2, 4> &vectors, double x, double y) {
        std::array<double,4> gradients;
        Vec2 pos = Vec2(x, y);
        double ceilX = std::floor(x) == x ? std::floor(x) + 1 : std::ceil(x);
        double ceilY = std::floor(y) == y ? std::floor(y) + 1 : std::ceil(y);



        gradients[0] = vectors[0].DotProduct(pos - Vec2(std::floor(x), std::floor(y)));
        gradients[1] = vectors[1].DotProduct(pos - Vec2(ceilX, std::floor(y)));
        gradients[2] = vectors[2].DotProduct(pos - Vec2(std::floor(x), ceilY));
        gradients[3] = vectors[3].DotProduct(pos - Vec2(ceilX, ceilY));

        return gradients;
    }

    double PerlinNoise::getPerlinNoise(int x, int y) {
        double sampleX = x / m_scale;
        double sampleY = y / m_scale;

        std::array<Vec2,4> vectors = getVectors(sampleX, sampleY);
        std::array<double,4> gradients = getGradients(vectors, sampleX, sampleY);

        return Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]), sampleX - std::floor(sampleX), sampleY - std::floor(sampleY)) * 0.5 + 0.5;
    }
    PerlinNoise::PerlinNoise(int width, int height, double scale, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = scale;
        m_seed = seed;
    }
}

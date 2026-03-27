#include "PerlinNoise.h"



namespace WorldMaker {
    const double PI = 3.14159265358979323846;
    std::array<Vec2,4> PerlinNoise::getVectors(double x, double y) {
        std::array<Vec2,4> result;
        double u = std::floor(x);
        double v = std::floor(y);
        result[0] = PRNG::randomVector2(u, v, m_seed).Normalized();
        result[1] = PRNG::randomVector2(u+1, v, m_seed).Normalized();
        result[2] = PRNG::randomVector2(u, v+1, m_seed).Normalized();
        result[3] = PRNG::randomVector2(u+1, v+1, m_seed).Normalized();

        // double angle = PRNG::randomHash(std::floor(x), std::floor(y), m_seed) * 2.0 * PI;
        // result[0] = Vec2(std::cos(angle), std::sin(angle));
        // angle = PRNG::randomHash(ceilX, std::floor(y), m_seed) * 2.0 * PI;
        // result[1] = Vec2(std::cos(angle), std::sin(angle));
        // angle = PRNG::randomHash(std::floor(x), ceilY, m_seed) * 2.0 * PI;
        // result[2] = Vec2(std::cos(angle), std::sin(angle));
        // angle = PRNG::randomHash(ceilX, ceilY, m_seed) * 2.0 * PI;
        // result[3] = Vec2(std::cos(angle), std::sin(angle));

        // for (int i = 0; i < result.size(); i++) {
        //     result[i] = Vec2(1.0, 0.0);
        // }
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
        if (std::floor(sampleX) == 1 && std::floor(sampleY) == 0) {
            std::cout << vectors[3] << std::endl;
        }
        if (std::floor(sampleX) == 1 && std::floor(sampleY) == 1) {
            std::cout << vectors[1] << std::endl;
        }

        std::array<double,4> gradients = getGradients(vectors, sampleX, sampleY);

        return Bilinear(Vec4(gradients[0], gradients[1], gradients[2], gradients[3]), sampleX - std::floor(sampleX), sampleY - std::floor(sampleY)) * 0.5 + 0.5;
        // return std::lerp(std::lerp(gradients[0], gradients[1], sampleX - std::floor(sampleX)), std::lerp(gradients[2], gradients[3], sampleX - std::floor(sampleX)) , sampleY - std::floor(sampleY)) * 0.5 + 0.5;
    }
    PerlinNoise::PerlinNoise(int width, int height, double scale, uint64_t seed) {
        m_height = height;
        m_width = width;
        m_scale = scale;
        m_seed = seed;
    }
}

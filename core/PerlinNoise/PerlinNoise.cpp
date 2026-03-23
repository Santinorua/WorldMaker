#include "PerlinNoise.h"

#include "../Utilities/Interpolations.h"


namespace WorldMaker {
    // std::vector<std::vector<Vec2>> PerlinNoise::getVectors(PRNG &prng) {
    //     std::vector<std::vector<Vec2>> vectors;
    //     for (int y = 0; y < m_height + 1; y++) {
    //         std::vector<Vec2> row;
    //         for (int x = 0; x < m_width + 1; x++) {
    //             double a = static_cast<double>(prng.nextNumber32() % 20001) / 10000.0 - 1.0;
    //             double b = static_cast<double>(prng.nextNumber32() % 20001) / 10000.0 - 1.0;
    //             Vec2 vec = Vec2(a, b);
    //             vec = vec.Normalized();
    //             row.push_back(vec);
    //         }
    //         vectors.push_back(row);
    //     }
    //     return vectors;
    // }
    // std::vector<std::vector<Vec4>> PerlinNoise::getGradients(std::vector<std::vector<Vec2>> &vectors) {
    //     std::vector<std::vector<Vec4>> gradients;
    //     for (int y = 0; y < m_height; y++) {
    //         std::vector<Vec4> row;
    //         for (int x = 0; x < m_width; x++) {
    //             double topLeft = vectors[y][x].DotProduct(TopLeft);
    //             double topRight = vectors[y][x+1].DotProduct(TopRight);
    //             double bottomLeft = vectors[y+1][x].DotProduct(BottomLeft);
    //             double bottomRight = vectors[y+1][x+1].DotProduct(BottomRight);
    //
    //             row.push_back(Vec4(topLeft, topRight, bottomLeft, bottomRight));
    //         }
    //         gradients.push_back(row);
    //     }
    //     return gradients;
    // }
    //
    // void PerlinNoise::getPerlinNoise(PRNG &prng) {
    //     std::vector<std::vector<Vec2>> vectors = getVectors(prng);
    //     std::vector<std::vector<Vec4>> gradients = getGradients(vectors);
    //     for (int y = 0; y < m_height; y++) {
    //         std::vector<double> row;
    //         row.reserve(m_width);
    //         for (int x = 0; x < m_width; x++) {
    //             row.push_back(Bilinear(gradients[y][x]));
    //         }
    //         m_perlinNoise.push_back(row);
    //     }
    // }
    // PerlinNoise::PerlinNoise(int width, int height, PRNG &prng) {
    //     m_height = height;
    //     m_width = width;
    //     m_perlinNoise.reserve(m_height);
    //     getPerlinNoise(prng);
    // }
}

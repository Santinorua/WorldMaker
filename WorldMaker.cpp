#include "WorldMaker.h"
#include <iostream>

using namespace WorldMaker;

const int width = 15;
const int height = 15;

int main() {
    PRNG prng;
    prng.seed = 42;
    PerlinNoise perlinNoise(width, height, prng);


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << perlinNoise.m_perlinNoise[y][x] << " ";
        }
        std::cout << std::endl;
    }
    getchar();
    return 0;
}


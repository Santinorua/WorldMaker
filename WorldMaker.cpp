#include "WorldMaker.h"
#include <iostream>

using namespace WorldMaker;

const int width = 6;
const int height = 15;

int main() {
    PerlinNoise perlinNoise(width, height, 10.0, 42);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << perlinNoise.getPerlinNoise(x,y) << " ";
            // std::cout << PRNG::randomVector2(x, y , 42) << " ";
        }
        std::cout << std::endl;
    }
    getchar();
    return 0;
}


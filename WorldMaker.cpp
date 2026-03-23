#include "WorldMaker.h"
#include <iostream>

using namespace WorldMaker;

const int width = 6;
const int height = 15;

int main() {
    PRNG prng;


    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::cout << prng.randomVector2(x,y, 42) << " ";
        }
        std::cout << std::endl;
    }
    getchar();
    return 0;
}


#pragma once
#include <cstdint>
#include <vector>
#include "Vec2.h"


namespace WorldMaker {
    struct PRNG {
        static uint64_t nextNumber64(uint64_t seed);
        static uint32_t nextNumber32(uint64_t seed);
        static Vec2 randomVector2(int x, int y, uint64_t seed);
        static double randomHash(int x, int y, uint64_t seed);
    };
}

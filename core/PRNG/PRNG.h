#pragma once
#include <cstdint>
#include "Vec2.h"
#include "Vec3.h"


namespace WorldMaker {
    struct PRNG {
        static uint64_t nextNumber64(uint64_t seed);
        static uint32_t nextNumber32(uint64_t seed);
        static uint32_t hash2D(int x, int y, uint64_t seed);
        static Vec2 randomVector2(int x, int y, uint64_t seed);
        static Vec3 randomVector3(int x, int y, int z, uint64_t seed);
    };
}

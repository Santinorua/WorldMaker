#pragma once
#include <cstdint>
#include <vector>
#include "Vec2.h"


namespace WorldMaker {
    struct PRNG {
        uint64_t nextNumber64(uint64_t seed);
        uint32_t nextNumber32(uint64_t seed);
        Vec2 randomVector2(int x, int y, seed);
    };
}

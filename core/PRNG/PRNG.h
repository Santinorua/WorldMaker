#pragma once
#include <cstdint>

struct PRNG {
    uint64_t seed;
    uint64_t nextNumber64();
    int nextNumber32();
};


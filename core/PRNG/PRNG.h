#pragma once
#include <cstdint>
#include <vector>

namespace WorldMaker {
    struct PRNG {
        uint64_t seed;
        uint64_t nextNumber64();
        uint32_t nextNumber32();
        std::vector<uint64_t> nextMultipleNumbers(int count);
    };
}
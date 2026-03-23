#include "PRNG.h"

namespace WorldMaker {
    uint64_t PRNG::nextNumber64(uint64_t seed) {
        // Xor Shift*
        uint64_t x = seed;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        return x * 0x2545F4914F6CDD1DULL;
    }

    uint32_t PRNG::nextNumber32(uint64_t seed) {
        // Convert the 64-bit number to a 32-bit number by taking the lower 32 bits
        return static_cast<uint32_t>(nextNumber64(seed) & 0xFFFFFFFF);
    }

    Vec2 PRNG::randomVector2(int x, int y, uint64_t seed) {
        uint64_t newSeed = static_cast<uint64_t>(nextNumber32(seed)) * static_cast<uint64_t>(x) * static_cast<uint64_t>(y);
        uint64_t r = nextNumber32(newSeed);
        double x = static_cast<double>(r % 200001) / 100000.0 - 1.0;
        double y = static_cast<double>(nextNumber32(r) % 200001) / 100000.0 - 1.0;
        return Vec2(x, y);
    }
}

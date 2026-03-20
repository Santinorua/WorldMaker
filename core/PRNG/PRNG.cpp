#include "PRNG.h"

namespace WorldMaker {
    uint64_t PRNG::nextNumber64() {
        // Xor Shift*
        uint64_t x = seed;
        x ^= x >> 12;
        x ^= x << 25;
        x ^= x >> 27;
        seed = x * 0x2545F4914F6CDD1DULL;
        return seed;
    }

    uint32_t PRNG::nextNumber32() {
        // Convert the 64-bit number to a 32-bit number by taking the lower 32 bits
        return static_cast<uint32_t>(nextNumber64() & 0xFFFFFFFF);
    }

    std::vector<uint64_t> PRNG::nextMultipleNumbers(int count) {
        std::vector<uint64_t> numbers;
        for (int i = 0; i < count; i++) {
            numbers.push_back(nextNumber64());
        }
        return numbers;
    }
}

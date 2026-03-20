#include "PRNG.h"

uint64_t PRNG::nextNumber64() {
    // Xor Shift*
    uint64_t x = seed;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    seed = x * 0x2545F4914F6CDD1DULL;
    return seed;
}

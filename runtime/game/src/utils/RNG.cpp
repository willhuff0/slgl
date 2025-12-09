#include <slgl/runtime/game/utils/RNG.hpp>

#include <chrono>

RNG::RNG(uint64_t seed) : engine(seed) { }

uint64_t RNG::GetTimeBasedSeed() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

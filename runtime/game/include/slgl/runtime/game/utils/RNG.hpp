#pragma once

#include <cstdint>
#include <random>
#include <concepts>

class RNG {
public:
    explicit RNG(uint64_t seed);

    template<std::integral T>
    T Sample(T min, T max);
    template<std::floating_point T>
    T Sample(T min, T max);

    static uint64_t GetTimeBasedSeed();

private:
    std::mt19937_64 engine;
};

template<std::integral T>
T RNG::Sample(T min, T max) {
    return std::uniform_int_distribution<T>(min, max)(engine);
}

template<std::floating_point T>
T RNG::Sample(T min, T max) {
    return std::uniform_real_distribution<T>(min, max)(engine);
}
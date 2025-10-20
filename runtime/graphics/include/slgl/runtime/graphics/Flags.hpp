#pragma once

#include <cstdint>
#include <bitset>

namespace SLGL::Graphics {
    template<typename T = uint8_t>
    class Flags {
    public:
        typedef std::bitset<sizeof(T) * 8> Bitset;

        Flags(T flags) : flags(flags) {}      // NOLINT(*-explicit-constructor)
        Flags(Bitset flags) : flags(flags) {} // NOLINT(*-explicit-constructor)

        inline Bitset GetBitset() { return flags; }

    private:
        Bitset flags;
    };
}
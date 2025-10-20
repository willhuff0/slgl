#pragma once

#include "Flags.hpp"

#include <cstdint>

namespace SLGL::Graphics {
    struct QueueFamily {
        class Capability {
        public:
            typedef Flags<uint8_t> Flags;

            static constexpr uint8_t None     = 0b00000000;
            static constexpr uint8_t Graphics = 0b00000001;
            static constexpr uint8_t Compute  = 0b00000010;
            static constexpr uint8_t Transfer = 0b00000100;
        };

        Capability::Flags capability = Capability::None;
        uint32_t count = 0;
    };

    typedef void Queue;
}
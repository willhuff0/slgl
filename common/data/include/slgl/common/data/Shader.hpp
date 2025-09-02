#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <variant>

namespace SLGL::Data {
    struct Shader {
        enum class Language : uint8_t {
            SPIRV = 0,
            GLSL = 1,
            HLSL = 2,
            WGSL = 3,
        };

        Language language = Language::SPIRV;
        std::variant<std::string, std::vector<uint32_t>> source;
    };
}
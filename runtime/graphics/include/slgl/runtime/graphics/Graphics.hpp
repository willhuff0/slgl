#pragma once

#include "Backend.hpp"

namespace SLGL::Graphics {
    enum class BackendType {
        DX12,
        Metal,
        Vulkan,
        WebGPU,
    };

    [[nodiscard]] Backend* GetBackend(BackendType backend);
    [[nodiscard]] Backend* GetBackend();
}
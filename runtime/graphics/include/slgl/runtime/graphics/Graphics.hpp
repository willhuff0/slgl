#pragma once

#include "Backend.hpp"

#include <vector>

namespace SLGL::Graphics {
    enum class BackendType {
        DX12,
        Metal,
        Vulkan,
        WebGPU,
    };

    [[nodiscard]] Backend* GetBackend(const std::vector<QueueFamily>& queues, BackendType backend);
    [[nodiscard]] Backend* GetBackend(const std::vector<QueueFamily>& queues);
    [[nodiscard]] Backend* GetBackend();
}
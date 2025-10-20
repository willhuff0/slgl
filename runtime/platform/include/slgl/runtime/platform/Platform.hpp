#pragma once

#include "Backend.hpp"
#include "Window.hpp"

namespace SLGL::Platform {
    enum class BackendType {
        GLFW,
        SDL3,
    };

    [[nodiscard]] Backend* GetBackend(BackendType backend);
    [[nodiscard]] Backend* GetBackend();
}
#pragma once

#include <slgl/runtime/graphics/Backend.hpp>
#include <slgl/runtime/platform/Platform.hpp>

#include <vector>
#include <string>

namespace SLGL::WSI {
    [[nodiscard]] Graphics::Surface::Ref CreateSurface(Graphics::Backend* gfx, Platform::Window::Ref window, bool vsync);
    [[nodiscard]] std::vector<std::string> GetRequiredExtensions(Platform::Backend* backend);
}
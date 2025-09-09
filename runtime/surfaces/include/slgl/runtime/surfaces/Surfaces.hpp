#pragma once

#include <slgl/runtime/graphics/Backend.hpp>
#include <slgl/runtime/platform/Window.hpp>

namespace SLGL::Surfaces {
    [[nodiscard]] Graphics::Surface::Ref CreateSurface(Graphics::Backend* gfx, Platform::Window::Ref window, bool vsync);
}
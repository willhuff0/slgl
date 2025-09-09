#pragma once

#include <slgl/runtime/graphics/Backend.hpp>
#include <slgl/runtime/platform/Window.hpp>

#include <memory>

namespace SLGL::Surfaces::SDL3_WebGPU {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* gfx, Platform::Window::Ref window, bool vsync);
}
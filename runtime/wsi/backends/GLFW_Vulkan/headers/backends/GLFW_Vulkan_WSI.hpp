#pragma once

#include <slgl/runtime/graphics/Backend.hpp>
#include <slgl/runtime/platform/Platform.hpp>

#include <memory>
#include <vector>
#include <string>

namespace SLGL::WSI::GLFW_Vulkan {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* gfx, Platform::Window::Ref window, bool vsync);
    std::vector<std::string> GetRequiredExtensions(Platform::Backend* backend);
}
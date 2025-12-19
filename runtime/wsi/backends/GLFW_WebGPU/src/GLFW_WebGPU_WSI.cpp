#include "backends/GLFW_WebGPU_WSI.hpp"

#include <slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp>
#include <slgl/runtime/graphics/backends/WebGPU/WebGPUSurface.hpp>
#include <slgl/runtime/platform/backends/GLFW/GLFWWindow.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glfw3webgpu.h>

#include <memory>
#include <cstdint>

namespace SLGL::WSI::GLFW_WebGPU {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* baseGfx, Platform::Window::Ref baseWindow, bool vsync) {
        auto gfx = dynamic_cast<Graphics::WebGPU::Backend*>(baseGfx);
        auto window = std::dynamic_pointer_cast<Platform::GLFW::Window>(baseWindow);

        auto handle = glfwCreateWindowWGPUSurface(gfx->getInstance(), window->getHandle());
        if (handle == nullptr) {
            throw std::runtime_error("Failed to create native glfw surface!");
        }

        return std::make_shared<Graphics::WebGPU::Surface>(window, handle, vsync, gfx->getAdapter(), gfx->getDevice());
    }

    std::vector<std::string> GetRequiredExtensions(Platform::Backend* backend) {
        return { };
    }
}

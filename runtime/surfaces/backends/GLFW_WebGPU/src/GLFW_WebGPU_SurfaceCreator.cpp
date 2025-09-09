#include "backends/GLFW_WebGPU_SurfaceCreator.hpp"

#include <slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp>
#include <slgl/runtime/graphics/backends/WebGPU/WebGPUSurface.hpp>
#include <slgl/runtime/platform/backends/GLFW/GLFWWindow.hpp>

#include <glfw3webgpu.h>

#include <memory>

namespace SLGL::Surfaces::GLFW_WebGPU {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* baseGfx, Platform::Window::Ref baseWindow, bool vsync) {
        auto gfx = dynamic_cast<Graphics::WebGPU::Backend*>(baseGfx);
        auto window = std::dynamic_pointer_cast<Platform::GLFW::Window>(baseWindow);

        auto handle = glfwCreateWindowWGPUSurface(gfx->getInstance(), window->getHandle());

        return std::make_shared<Graphics::WebGPU::Surface>(window, handle, vsync,
                                                           gfx->getAdapter(), gfx->getDevice());
    }
}

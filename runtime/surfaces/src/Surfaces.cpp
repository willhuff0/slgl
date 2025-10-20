#include <slgl/runtime/surfaces/Surfaces.hpp>

#include <stdexcept>

#if defined(SLGL_PLATFORM_BACKEND_SDL3)
#include <slgl/runtime/platform/backends/SDL3/SDL3Window.hpp>
#endif

#if defined(SLGL_PLATFORM_BACKEND_GLFW)
#include <slgl/runtime/platform/backends/GLFW/GLFWWindow.hpp>
#endif

// ---

#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
#include <slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp>
#endif

// ===

#if defined(SLGL_PLATFORM_BACKEND_SDL3)

#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
#include "backends/SDL3_WebGPU_SurfaceCreator.hpp"
#endif

#endif

// ---

#if defined(SLGL_PLATFORM_BACKEND_GLFW)

#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
#include "backends/GLFW_WebGPU_SurfaceCreator.hpp"
#endif

#endif

namespace SLGL::Surfaces {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* gfx, Platform::Window::Ref window, bool vsync) {
#if defined(SLGL_PLATFORM_BACKEND_SDL3) && defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        if (dynamic_cast<Platform::SDL3::Window*>(window.get()) && dynamic_cast<Graphics::WebGPU::Backend*>(gfx)) {
            return SDL3_WebGPU::CreateSurface(gfx, window, vsync);
        }
#endif
#if defined(SLGL_PLATFORM_BACKEND_GLFW) && defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        if (dynamic_cast<Platform::GLFW::Window*>(window.get()) && dynamic_cast<Graphics::WebGPU::Backend*>(gfx)) {
            return GLFW_WebGPU::CreateSurface(gfx, window, vsync);
        }
#endif

        throw std::runtime_error("No available surface creator for this platform/graphics backend combination.");
    }
}
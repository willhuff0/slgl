#include "backends/SDL3_WebGPU_SurfaceCreator.hpp"

#include <slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp>
#include <slgl/runtime/graphics/backends/WebGPU/WebGPUSurface.hpp>
#include <slgl/runtime/platform/backends/SDL3/SDL3Window.hpp>

#include <sdl3webgpu.h>

#include <memory>

namespace SLGL::Surfaces::SDL3_WebGPU {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* baseGfx, Platform::Window::Ref baseWindow, bool vsync) {
        auto gfx = dynamic_cast<Graphics::WebGPU::Backend*>(baseGfx);
        auto window = std::dynamic_pointer_cast<Platform::SDL3::Window>(baseWindow);

        auto handle = SDL_GetWGPUSurface(gfx->getInstance(), window->getHandle());

        return std::make_shared<Graphics::WebGPU::Surface>(window, handle, vsync,
                                                           gfx->getAdapter(), gfx->getDevice());
    }
}
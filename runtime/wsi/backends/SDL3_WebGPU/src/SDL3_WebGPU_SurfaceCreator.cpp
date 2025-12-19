#include "backends/SDL3_WebGPU_WSI.hpp"

#include <slgl/runtime/graphics/backends/WebGPU/WebGPUBackend.hpp>
#include <slgl/runtime/graphics/backends/WebGPU/WebGPUSurface.hpp>
#include <slgl/runtime/platform/backends/SDL3/SDL3Window.hpp>

#include <SDL3/SDL_vulkan.h>
#include <sdl3webgpu.h>

#include <memory>
#include <cstdint>

namespace SLGL::Surfaces::SDL3_WebGPU {
    Graphics::Surface::Ref CreateSurface(Graphics::Backend* baseGfx, Platform::Window::Ref baseWindow, bool vsync) {
        auto gfx = dynamic_cast<Graphics::WebGPU::Backend*>(baseGfx);
        auto window = std::dynamic_pointer_cast<Platform::SDL3::Window>(baseWindow);

        auto handle = SDL_GetWGPUSurface(gfx->getInstance(), window->getHandle());

        return std::make_shared<Graphics::WebGPU::Surface>(window, handle, vsync, gfx->getAdapter(), gfx->getDevice());
    }

    std::vector<std::string> GetRequiredExtensions(Platform::Backend* backend) {
        uint32_t count;
        const char* const* extensions = SDL_Vulkan_GetInstanceExtensions(&count);

        std::vector<std::string> result;
        result.reserve(count);
        for(uint32_t i = 0; i < count; i++) {
            result.emplace_back(extensions[i]);
        }

        return result;
    }
}
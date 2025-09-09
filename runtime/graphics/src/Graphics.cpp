#include <slgl/runtime/graphics/Graphics.hpp>

#include <stdexcept>

#if defined(SLGL_GRAPHICS_BACKEND_DX12)
#include "backends/DX12/DX12Backend.hpp"
#endif
#if defined(SLGL_GRAPHICS_BACKEND_METAL)
#include "backends/Metal/MetalBackend.hpp"
#endif
#if defined(SLGL_GRAPHICS_BACKEND_VULKAN)
#include "backends/Vulkan/VulkanBackend.hpp"
#endif
#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
#include "backends/WebGPU/WebGPUBackend.hpp"
#endif

namespace SLGL::Graphics {
    static Backend* GetDX12Backend() {
#if defined(SLGL_GRAPHICS_BACKEND_DX12)
#error DX12 not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetMetalBackend() {
#if defined(SLGL_GRAPHICS_BACKEND_METAL)
#error Metal not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetVulkanBackend() {
#if defined(SLGL_GRAPHICS_BACKEND_VULKAN)
#error Vulkan not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetWebGPUBackend() {
#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        static WebGPU::Backend gfx;
        return &gfx;
#else
        return nullptr;
#endif
    }

    Backend* GetBackend(BackendType backend) {
        switch (backend) {
            case BackendType::DX12:
                return GetDX12Backend();
            case BackendType::Metal:
                return GetMetalBackend();
            case BackendType::Vulkan:
                return GetVulkanBackend();
            case BackendType::WebGPU:
                return GetWebGPUBackend();
            default:
                throw std::runtime_error("");
        }
    }
    Backend* GetBackend() {
#if defined(SLGL_GRAPHICS_BACKEND_DX12)
        return GetGFX(GraphicsBackendType::DX12);
#elif defined(SLGL_GRAPHICS_BACKEND_METAL)
        return GetGFX(GraphicsBackendType::Metal);
#elif defined(SLGL_GRAPHICS_BACKEND_VULKAN)
        return GetGFX(GraphicsBackendType::Vulkan);
#elif defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        return GetBackend(BackendType::WebGPU);
#else
#error No available graphics backends!
#endif
    }
}
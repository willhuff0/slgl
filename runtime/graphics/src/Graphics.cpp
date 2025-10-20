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
    static Backend* GetDX12Backend(const std::vector<QueueFamily>& queueFamilies) {
#if defined(SLGL_GRAPHICS_BACKEND_DX12)
#error DX12 not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetMetalBackend(const std::vector<QueueFamily>& queueFamilies) {
#if defined(SLGL_GRAPHICS_BACKEND_METAL)
#error Metal not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetVulkanBackend(const std::vector<QueueFamily>& queueFamilies) {
#if defined(SLGL_GRAPHICS_BACKEND_VULKAN)
#error Vulkan not implemented!
#else
        return nullptr;
#endif
    }
    static Backend* GetWebGPUBackend(const std::vector<QueueFamily>& queueFamilies) {
#if defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        static WebGPU::Backend gfx(queueFamilies);
        return &gfx;
#else
        return nullptr;
#endif
    }

    Backend* GetBackend(const std::vector<QueueFamily>& queueFamilies, BackendType backend) {
        switch (backend) {
            case BackendType::DX12:
                return GetDX12Backend(queueFamilies);
            case BackendType::Metal:
                return GetMetalBackend(queueFamilies);
            case BackendType::Vulkan:
                return GetVulkanBackend(queueFamilies);
            case BackendType::WebGPU:
                return GetWebGPUBackend(queueFamilies);
            default:
                throw std::runtime_error("");
        }
    }
    Backend* GetBackend(const std::vector<QueueFamily>& queueFamilies) {
#if defined(SLGL_GRAPHICS_BACKEND_DX12)
        return GetGFX(queues, GraphicsBackendType::DX12);
#elif defined(SLGL_GRAPHICS_BACKEND_METAL)
        return GetGFX(queues, GraphicsBackendType::Metal);
#elif defined(SLGL_GRAPHICS_BACKEND_VULKAN)
        return GetGFX(queues, GraphicsBackendType::Vulkan);
#elif defined(SLGL_GRAPHICS_BACKEND_WEBGPU)
        return GetBackend(queueFamilies, BackendType::WebGPU);
#else
#error No available graphics backends!
#endif
    }
    Backend* GetBackend() {
        return GetBackend({
            QueueFamily {
                .capability = QueueFamily::Capability::Graphics | QueueFamily::Capability::Transfer,
                .count = 1,
            }
        });
    }
}
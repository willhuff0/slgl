#pragma once

#include <webgpu/webgpu.hpp>

#include <unordered_map>

using namespace wgpu;

namespace SLGL::Graphics::WebGPU {
    static std::unordered_map<WGPUAdapterType, const char*> wgpuAdapterTypeNames {
            {AdapterType::CPU,             "CPU  - - - - - -"},
            {AdapterType::DiscreteGPU,     "Discrete GPU - -"},
            {AdapterType::IntegratedGPU,   "Integrated GPU -"},
    };

    static std::unordered_map<WGPUBackendType, const char*> wgpuBackendTypeNames {
            {BackendType::WebGPU,          "WebGPU - - - - -"},
            {BackendType::D3D11,           "DirectX 11 - - -"},
            {BackendType::D3D12,           "DirectX 12 - - -"},
            {BackendType::Metal,           "Metal  - - - - -"},
            {BackendType::Vulkan,          "Vulkan - - - - -"},
            {BackendType::OpenGL,          "OpenGL - - - - -"},
            {BackendType::OpenGLES,        "OpenGLES - - - -"},
    };

    static std::unordered_map<WGPUPresentMode, const char*> wgpuPresentModeNames {
            {PresentMode::Mailbox,         "Mailbox  - - - - Fast Vsync"},
            {PresentMode::FifoRelaxed,     "FIFO Relaxed - - Adaptive Vsync"},
            {PresentMode::Fifo,            "FIFO - - - - - - Vsync"},
            {PresentMode::Immediate,       "Immediate  - - - Vsync Off"},
    };

    void DebugPrintAdapter(Adapter& adapter);
    void DebugPrintPresentMode(WGPUPresentMode presentMode);
}
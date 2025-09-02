#pragma once

#include <slgl/runtime/platform/IWindow.hpp>
#include <slgl/runtime/graphics/MipmapGenerator.hpp>

#include "GFX/WebGPUSurface.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics::WebGPU {
    class Context {
    public:
        ~Context();

        Surface* surface = nullptr;
        wgpu::Device device;
    };
}
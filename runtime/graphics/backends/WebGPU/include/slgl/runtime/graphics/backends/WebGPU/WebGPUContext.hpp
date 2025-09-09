#pragma once

#include <slgl/runtime/platform/Window.hpp>
#include <slgl/runtime/graphics/MipmapGenerator.hpp>

#include "WebGPUSurface.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics::WebGPU {
    class Context {
    public:
        ~Context();

        wgpu::Device device;
    };
}
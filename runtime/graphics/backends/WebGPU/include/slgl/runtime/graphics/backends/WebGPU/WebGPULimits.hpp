#pragma once

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics::WebGPU {
    wgpu::Limits GetRequiredLimits(wgpu::Adapter& adapter);
}
#include "WebGPULimits.hpp"

namespace SLGL::Graphics {
    wgpu::Limits WebGPU::GetRequiredLimits(wgpu::Adapter& adapter) {
        wgpu::Limits required = wgpu::Default;
        required.maxVertexAttributes = 3;
        required.maxVertexBuffers = 3;
        required.maxBufferSize = 4096;
        required.maxVertexBufferArrayStride = 20;

        required.maxInterStageShaderVariables = 6;

        required.maxBindGroups = 4;
        required.maxBindingsPerBindGroup = 5;
        required.maxUniformBuffersPerShaderStage = 1;
        required.maxUniformBufferBindingSize = 80;

        required.maxSamplersPerShaderStage = 5;

        required.maxTextureDimension1D = 3840;
        required.maxTextureDimension2D = 3840;
        required.maxTextureArrayLayers = 1;

        return required;
    }
}
#include "backends/WebGPU/WebGPUBackend.hpp"

#include "backends/WebGPU/WebGPUBuffer.hpp"
#include "backends/WebGPU/WebGPUSampler.hpp"
#include "backends/WebGPU/WebGPUTexture.hpp"
#include "backends/WebGPU/WebGPUBindSet.hpp"
#include "backends/WebGPU/WebGPUShaderModule.hpp"
#include "backends/WebGPU/WebGPUShaderPipelineLayout.hpp"
#include "backends/WebGPU/WebGPUShaderRenderPipeline.hpp"

#include <string>

namespace SLGL::Graphics {
    Graphics::Buffer::Builder WebGPU::Backend::CreateBuffer() {
        static WebGPU::Buffer::Builder impl(ctx);
        return Graphics::Buffer::Builder(&impl);
    }

    Graphics::Sampler::Builder WebGPU::Backend::CreateSampler() {
        static WebGPU::Sampler::Builder impl(ctx);
        return Graphics::Sampler::Builder(&impl);
    }

    Graphics::Texture::Builder WebGPU::Backend::CreateTexture() {
        static WebGPU::Texture::Builder impl(ctx);
        return Graphics::Texture::Builder(&impl);
    }

    Graphics::Texture::Ref WebGPU::Backend::CreateTexture(const std::string& label, Graphics::Queue* _queue, const Data::Image& image, int mipLevels) {
        return WebGPU::Texture::FromData(this, _queue, label, image, mipLevels);
    }
    Graphics::Texture::Ref WebGPU::Backend::CreateTexture(const std::string& label, Graphics::Queue* _queue, const Data::Image& image) {
        return WebGPU::Texture::FromData(this, _queue, label, image);
    }

    Graphics::BindSet::Layout::Builder WebGPU::Backend::CreateBindSetLayout() {
        static WebGPU::BindSet::Layout::Builder impl(ctx);
        return Graphics::BindSet::Layout::Builder(&impl);
    }

    Graphics::ShaderModule::Source::Ref WebGPU::Backend::CreateShaderSource(const Data::Shader& shader) {
        return WebGPU::ShaderModule::Source::FromData(shader);
    }

    Graphics::ShaderModule::Builder WebGPU::Backend::CreateShaderModule() {
        static WebGPU::ShaderModule::Builder impl(ctx);
        return Graphics::ShaderModule::Builder(&impl);
    }

    Graphics::ShaderPipeline::Layout::Builder WebGPU::Backend::CreateShaderPipelineLayout() {
        static WebGPU::ShaderPipeline::Layout::Builder impl(ctx);
        return Graphics::ShaderPipeline::Layout::Builder(&impl);
    }

    Graphics::ShaderPipeline::Render::Builder WebGPU::Backend::CreateShaderRenderPipeline(Graphics::ShaderPipeline::Layout::Ref layout) {
        static WebGPU::ShaderPipeline::Render::Builder impl(ctx);
        return Graphics::ShaderPipeline::Render::Builder(&impl, layout);
    }
}
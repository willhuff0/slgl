#include "GFX/WebGPUGFX.hpp"

#include "GFX/WebGPUBuffer.hpp"
#include "GFX/WebGPUSampler.hpp"
#include "GFX/WebGPUTexture.hpp"
#include "GFX/WebGPUBindSet.hpp"
#include "GFX/WebGPUShaderModule.hpp"
#include "GFX/WebGPUShaderPipelineLayout.hpp"
#include "GFX/WebGPUShaderRenderPipeline.hpp"

#include <string>

namespace SLGL::Graphics {
    Graphics::Buffer::Builder WebGPU::GFX::CreateBuffer() {
        static WebGPU::Buffer::Builder impl(ctx);
        return Graphics::Buffer::Builder(&impl);
    }

    Graphics::Sampler::Builder WebGPU::GFX::CreateSampler() {
        static WebGPU::Sampler::Builder impl(ctx);
        return Graphics::Sampler::Builder(&impl);
    }

    Graphics::Texture::Builder WebGPU::GFX::CreateTexture() {
        static WebGPU::Texture::Builder impl(ctx);
        return Graphics::Texture::Builder(&impl);
    }

    Graphics::Texture::Ref WebGPU::GFX::CreateTexture(const std::string& label, Graphics::Queue* _queue, const Data::Image& image, int mipLevels) {
        return WebGPU::Texture::FromData(this, _queue, label, image, mipLevels);
    }
    Graphics::Texture::Ref WebGPU::GFX::CreateTexture(const std::string& label, Graphics::Queue* _queue, const Data::Image& image) {
        return WebGPU::Texture::FromData(this, _queue, label, image);
    }

    Graphics::BindSet::Layout::Builder WebGPU::GFX::CreateBindSetLayout() {
        static WebGPU::BindSet::Layout::Builder impl(ctx);
        return Graphics::BindSet::Layout::Builder(&impl);
    }

    Graphics::ShaderModule::Source::Ref WebGPU::GFX::CreateShaderSource(const Data::Shader& shader) {
        return WebGPU::ShaderModule::Source::FromData(shader);
    }

    Graphics::ShaderModule::Builder WebGPU::GFX::CreateShaderModule() {
        static WebGPU::ShaderModule::Builder impl(ctx);
        return Graphics::ShaderModule::Builder(&impl);
    }

    Graphics::ShaderPipeline::Layout::Builder WebGPU::GFX::CreateShaderPipelineLayout() {
        static WebGPU::ShaderPipeline::Layout::Builder impl(ctx);
        return Graphics::ShaderPipeline::Layout::Builder(&impl);
    }

    Graphics::ShaderPipeline::Render::Builder WebGPU::GFX::CreateShaderRenderPipeline(Graphics::ShaderPipeline::Layout::Ref layout) {
        static WebGPU::ShaderPipeline::Render::Builder impl(ctx);
        return Graphics::ShaderPipeline::Render::Builder(&impl, layout);
    }
}
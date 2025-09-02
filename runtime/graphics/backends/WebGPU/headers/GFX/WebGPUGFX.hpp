#pragma once

#include <slgl/runtime/graphics/GFX.hpp>

#include "../WebGPUContext.hpp"
#include "WebGPUSurface.hpp"

#include <slgl/common/data/Data.hpp>

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics::WebGPU {
    class GFX : public Graphics::GFX, public Graphics::SurfaceSizeSubject::Observer {
    public:
        void Initialize(Graphics::SurfaceFactory* surfaceFactory, bool vsync) override;
        ~GFX() override;

        [[nodiscard]] Graphics::Surface* GetSurface() override;

        Graphics::Buffer::Builder CreateBuffer() override;
        Graphics::Sampler::Builder CreateSampler() override;
        Graphics::Texture::Builder CreateTexture() override;
        Graphics::Texture::Ref CreateTexture(const std::string& label, Graphics::Queue* queue, const SLGL::Data::Image& image, int mipLevels) override;
        Graphics::Texture::Ref CreateTexture(const std::string& label, Graphics::Queue* queue, const SLGL::Data::Image& image) override;
        Graphics::BindSet::Layout::Builder CreateBindSetLayout() override;
        Graphics::ShaderModule::Source::Ref CreateShaderSource(const SLGL::Data::Shader& shader) override;
        Graphics::ShaderModule::Builder CreateShaderModule() override;
        Graphics::ShaderPipeline::Layout::Builder CreateShaderPipelineLayout() override;
        Graphics::ShaderPipeline::Render::Builder CreateShaderRenderPipeline(Graphics::ShaderPipeline::Layout::Ref layout) override;
        Graphics::CommandEncoder::Ref CreateCommandEncoder(const std::string& label) override;

        void SubmitCommands(Graphics::Queue* queue, Graphics::CommandBuffer::Ref commandBuffer) override;
        void SubmitCommands(Graphics::Queue* queue, const std::vector<Graphics::CommandBuffer::Ref> &commandBuffers) override;

        void Tick() override;

        [[nodiscard]] Graphics::MipmapGenerator* GetMipmapGenerator() const;

        void onSurfaceSizeChanged(const int& width, const int& height) override;

    private:
        Graphics::SurfaceSizeSubject* surfaceSizeSubject = nullptr;

        wgpu::Instance instance;
        wgpu::Queue queue;

        Context ctx;

        std::unique_ptr<Graphics::MipmapGenerator> mipmapGenerator = nullptr;
    };
}

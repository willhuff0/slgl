#pragma once

#include <slgl/runtime/graphics/Backend.hpp>

#include "WebGPUContext.hpp"

#include <slgl/common/data/Data.hpp>
#include <slgl/runtime/graphics/MipmapGenerator.hpp>

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics::WebGPU {
    class Backend : public Graphics::Backend {
    public:
        Backend();
        ~Backend() override;

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

        [[nodiscard]] wgpu::Instance getInstance() const;
        [[nodiscard]] wgpu::Adapter getAdapter() const;
        [[nodiscard]] wgpu::Device getDevice() const;

    private:
        wgpu::Instance instance;
        wgpu::Adapter adapter;
        wgpu::Queue queue;

        Context ctx;

        std::unique_ptr<Graphics::MipmapGenerator> mipmapGenerator = nullptr;

        bool isDeviceLossIntentional = false;
    };
}

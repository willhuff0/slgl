#pragma once

#include "Surface.hpp"
#include "SurfaceFactory.hpp"

#include "Queue.hpp"
#include "RenderEncoder.hpp"

#include "Buffer.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "BindSet.hpp"
#include "ShaderModule.hpp"
#include "ShaderPipeline.hpp"
#include "CommandBuffer.hpp"
#include "CommandEncoder.hpp"

#include <slgl/common/data/Data.hpp>

#include <functional>
#include <vector>

namespace SLGL::Graphics {
    class GFX {
    public:
        virtual void Initialize(SurfaceFactory* surfaceFactory, bool vsync) = 0;
        virtual ~GFX() = default;

        [[nodiscard]] virtual Surface* GetSurface() = 0;

        virtual Buffer::Builder CreateBuffer() = 0;
        virtual Sampler::Builder CreateSampler() = 0;
        virtual Texture::Builder CreateTexture() = 0;
        virtual Texture::Ref CreateTexture(const std::string& label, Queue* queue, const Data::Image& image, int mipLevels) = 0;
        virtual Texture::Ref CreateTexture(const std::string& label, Queue* queue, const Data::Image& image) = 0;
        virtual BindSet::Layout::Builder CreateBindSetLayout() = 0;
        virtual ShaderModule::Source::Ref CreateShaderSource(const Data::Shader& shader) = 0;
        virtual ShaderModule::Builder CreateShaderModule() = 0;
        virtual ShaderPipeline::Layout::Builder CreateShaderPipelineLayout() = 0;
        virtual ShaderPipeline::Render::Builder CreateShaderRenderPipeline(ShaderPipeline::Layout::Ref layout) = 0;
        virtual CommandEncoder::Ref CreateCommandEncoder(const std::string& label) = 0;

        virtual void SubmitCommands(Queue* queue, CommandBuffer::Ref commandBuffer) = 0;
        virtual void SubmitCommands(Queue* queue, const std::vector<CommandBuffer::Ref>& commandBuffers) = 0;

        virtual void Tick() = 0;
    };
}
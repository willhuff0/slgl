#pragma once

#include "../WebGPUContext.hpp"

#include <slgl/runtime/graphics/ShaderPipeline.hpp>

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU::ShaderPipeline {
class Render : public Graphics::ShaderPipeline::Render {
public:
    class Builder : public Graphics::ShaderPipeline::Render::Builder::Impl {
    public:
        explicit Builder(Context& ctx);

        Ref Build(Graphics::ShaderPipeline::Render::Builder &builder) override;

        private:
            Context& ctx;
        };

        Render(wgpu::RenderPipeline renderPipeline, std::string label);
        ~Render() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::RenderPipeline GetHandle();
        const std::string& GetLabel() override;

    private:
        wgpu::RenderPipeline renderPipeline;
        std::string label;
    };
}

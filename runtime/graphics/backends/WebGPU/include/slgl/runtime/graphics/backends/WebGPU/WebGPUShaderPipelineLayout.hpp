#pragma once

#include <slgl/runtime/graphics/ShaderPipeline.hpp>

#include "backends/WebGPU/WebGPUContext.hpp"

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU::ShaderPipeline {
    class Layout : public Graphics::ShaderPipeline::Layout {
    public:
        class Builder : public Graphics::ShaderPipeline::Layout::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::ShaderPipeline::Layout::Builder &builder) override;

        private:
            Context& ctx;
        };

        Layout(wgpu::PipelineLayout pipelineLayout, std::string label);
        ~Layout() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::PipelineLayout GetHandle();
        const std::string &GetLabel() override;

    private:
        wgpu::PipelineLayout pipelineLayout;
        std::string label;
    };
}

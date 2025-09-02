#include "GFX/WebGPUShaderPipelineLayout.hpp"

#include "GFX/WebGPUBindSet.hpp"

#include <webgpu/webgpu.hpp>

#include <vector>

namespace SLGL::Graphics {
    WebGPU::ShaderPipeline::Layout::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::ShaderPipeline::Layout::Ref WebGPU::ShaderPipeline::Layout::Builder::Build(Graphics::ShaderPipeline::Layout::Builder& builder) {
        wgpu::PipelineLayoutDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.bindGroupLayoutCount = builder.getBindSetLayouts().size();
        std::vector<WGPUBindGroupLayout> bindGroupLayouts;
        bindGroupLayouts.reserve(desc.bindGroupLayoutCount);
        for (const auto& bindSetLayout : builder.getBindSetLayouts()) {
            bindGroupLayouts.push_back(dynamic_cast<BindSet::Layout*>(bindSetLayout.get())->GetHandle());
        }
        desc.bindGroupLayouts = bindGroupLayouts.data();
        return std::make_shared<Layout>(ctx.device.createPipelineLayout(desc), builder.getLabel());
    }

    WebGPU::ShaderPipeline::Layout::Layout(wgpu::PipelineLayout pipelineLayout, std::string label)
        : pipelineLayout(pipelineLayout), label(std::move(label)) { }

    WebGPU::ShaderPipeline::Layout::~Layout() {
        pipelineLayout.release();
    }

    void WebGPU::ShaderPipeline::Layout::SetLabel(const std::string &newLabel) {
        label = newLabel;
        pipelineLayout.setLabel(wgpu::StringView(label));
    }

    wgpu::PipelineLayout WebGPU::ShaderPipeline::Layout::GetHandle() { return pipelineLayout; }
    const std::string &WebGPU::ShaderPipeline::Layout::GetLabel() { return label; }
}
#include "GFX/WebGPUShaderModule.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    WebGPU::ShaderModule::Source::Ref WebGPU::ShaderModule::Source::FromData(const Data::Shader& resource) {
        switch (resource.language) {
            case SLGL::Data::Shader::Language::SPIRV:
                return std::make_shared<SPIRVSource>(std::get<std::vector<uint32_t>>(resource.source));
            case SLGL::Data::Shader::Language::WGSL:
                return std::make_shared<WGSLSource>(std::get<std::string>(resource.source));
            default: throw std::runtime_error("Shader language is not supported!");
        }
    }

    // WGSL
    WebGPU::ShaderModule::WGSLSource::WGSLSource(const std::string &code) {
        wgsl.code = wgpu::StringView(code);
    }
    WGPUChainedStruct* WebGPU::ShaderModule::WGSLSource::GetChain() { return &wgsl.chain; }

    // SPIR-V
    WebGPU::ShaderModule::SPIRVSource::SPIRVSource(const std::vector<uint32_t>& code) {
        spirv.codeSize = code.size();
        spirv.code = code.data();
    }
    WGPUChainedStruct* WebGPU::ShaderModule::SPIRVSource::GetChain() { return &spirv.chain; }

    // Shader module
    WebGPU::ShaderModule::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }

    Graphics::ShaderModule::Ref WebGPU::ShaderModule::Builder::Build(Graphics::ShaderModule::Builder &builder) {
        wgpu::ShaderModuleDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.nextInChain = dynamic_cast<Source*>(builder.getSource().get())->GetChain();
        return std::make_shared<ShaderModule>(ctx.device.createShaderModule(desc), builder.getLabel());
    }

    WebGPU::ShaderModule::ShaderModule(wgpu::ShaderModule shaderModule, std::string label) : shaderModule(shaderModule),
        label(std::move(label)) { }
    WebGPU::ShaderModule::~ShaderModule() {
        shaderModule.release();
    }

    void WebGPU::ShaderModule::SetLabel(const std::string &newLabel) {
        label = newLabel;
        shaderModule.setLabel(wgpu::StringView(label));
    }

    wgpu::ShaderModule WebGPU::ShaderModule::GetHandle() { return shaderModule; }
    const std::string &WebGPU::ShaderModule::GetLabel() { return label; }
}
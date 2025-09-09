#pragma once

#include <slgl/runtime/graphics/ShaderModule.hpp>

#include "backends/WebGPU/WebGPUContext.hpp"

#include <slgl/common/data/Shader.hpp>

#include <webgpu/webgpu.hpp>

#include <string>
#include <vector>

namespace SLGL::Graphics::WebGPU {
    class ShaderModule : public Graphics::ShaderModule {
    public:
        class Source : public Graphics::ShaderModule::Source {
        public:
            ~Source() override = default;

            static Source::Ref FromData(const Data::Shader& resource);

            virtual WGPUChainedStruct* GetChain() = 0;
        };
        class WGSLSource : public Source {
        public:
            explicit WGSLSource(const std::string& code);

            WGPUChainedStruct* GetChain() override;

        private:
            wgpu::ShaderSourceWGSL wgsl = wgpu::Default;
        };
        class SPIRVSource : public Source {
        public:
            explicit SPIRVSource(const std::vector<uint32_t>& code);

            WGPUChainedStruct* GetChain() override;

        private:
            wgpu::ShaderSourceSPIRV spirv = wgpu::Default;
        };

        class Builder : public Graphics::ShaderModule::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::ShaderModule::Builder &builder) override;

        private:
            Context& ctx;
        };

        ShaderModule(wgpu::ShaderModule shaderModule, std::string label);
        ~ShaderModule() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::ShaderModule GetHandle();
        const std::string &GetLabel() override;

    private:
        wgpu::ShaderModule shaderModule;
        std::string label;
    };
}

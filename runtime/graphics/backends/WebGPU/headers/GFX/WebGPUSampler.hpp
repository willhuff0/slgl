#pragma once

#include "../WebGPUContext.hpp"

#include <slgl/runtime/graphics/Sampler.hpp>

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU {
    class Sampler : public Graphics::Sampler {
    public:
        class Builder : public Graphics::Sampler::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref Build(Graphics::Sampler::Builder &builder) override;

        private:
            Context& ctx;
        };

        Sampler(wgpu::Sampler sampler, std::string label);
        ~Sampler() override;

        void SetLabel(const std::string &newLabel) override;

        wgpu::Sampler GetHandle();
        const std::string &GetLabel() override;

    private:
        wgpu::Sampler sampler;
        std::string label;
    };
}

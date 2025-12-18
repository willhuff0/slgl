#pragma once

#include <slgl/runtime/graphics/RenderBundle.hpp>

#include "backends/WebGPU/WebGPUContext.hpp"

#include <webgpu/webgpu.hpp>

#include <string>

namespace SLGL::Graphics::WebGPU {
    class RenderBundle : public Graphics::RenderBundle {
    public:
        class Builder : public Graphics::RenderBundle::Builder::Impl {
        public:
            explicit Builder(Context& ctx);

            Ref BuildAndEncode(Graphics::RenderBundle::Builder& builder, const std::function<void(Graphics::RenderBundleEncoder*)>& func) override;

        private:
            Context& ctx;
        };

        RenderBundle(wgpu::RenderBundle renderBundle, std::string label);
        ~RenderBundle() override;

        void SetLabel(const std::string &newLabel) override;

        [[nodiscard]] wgpu::RenderBundle GetHandle() const;
        [[nodiscard]] const std::string& GetLabel() const override;

    private:
        wgpu::RenderBundle renderBundle;
        std::string label;
    };
}
#include "backends/WebGPU/WebGPURenderBundle.hpp"

#include "backends/WebGPU/WebGPURenderBundleEncoder.hpp"
#include "backends/WebGPU/WebGPUConverters.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>

namespace SLGL::Graphics {
    WebGPU::RenderBundle::Builder::Builder(SLGL::Graphics::WebGPU::Context& ctx) : ctx(ctx) { }
    WebGPU::RenderBundle::Ref WebGPU::RenderBundle::Builder::BuildAndEncode(Graphics::RenderBundle::Builder& builder, const std::function<void(Graphics::RenderBundleEncoder*)>& func) {
        wgpu::RenderBundleEncoderDescriptor encoderDesc = wgpu::Default;
        std::string encoderLabel = builder.getLabel() + " Encoder";
        encoderDesc.label = wgpu::StringView(encoderLabel);
        encoderDesc.colorFormatCount = builder.getColorFormats().size();
        std::vector<WGPUTextureFormat> colorFormats;
        colorFormats.reserve(encoderDesc.colorFormatCount);
        for (const auto& colorFormat : builder.getColorFormats()) {
            colorFormats.push_back(convertTextureFormat(colorFormat.first, colorFormat.second));
        }
        encoderDesc.colorFormats = colorFormats.data();
        if (builder.getDepthStencilFormat().has_value()) {
            encoderDesc.depthStencilFormat = convertTextureFormat(builder.getDepthStencilFormat().value(), Graphics::Texture::ColorSpace::Linear);
        }
        encoderDesc.sampleCount = builder.getSampleCount();
        encoderDesc.depthReadOnly = builder.isDepthReadOnly();
        encoderDesc.stencilReadOnly = builder.isStencilReadOnly();

        wgpu::RenderBundleEncoder encoder = ctx.device.createRenderBundleEncoder(encoderDesc);
        WebGPU::RenderBundleEncoder renderBundleEncoder(encoder);
        func(&renderBundleEncoder);

        wgpu::RenderBundleDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        return std::make_shared<RenderBundle>(encoder.finish(desc), builder.getLabel());
    }

    WebGPU::RenderBundle::RenderBundle(wgpu::RenderBundle renderBundle, std::string label)
            : renderBundle(renderBundle), label(std::move(label)) {}
    WebGPU::RenderBundle::~RenderBundle() {
        renderBundle.release();
    }

    wgpu::RenderBundle WebGPU::RenderBundle::GetHandle() const {
        return renderBundle;
    }

    const std::string& WebGPU::RenderBundle::GetLabel() const {
        return label;
    }

    void WebGPU::RenderBundle::SetLabel(const std::string& newLabel) {
        label = newLabel;
        renderBundle.setLabel(wgpu::StringView(label));
    }
}
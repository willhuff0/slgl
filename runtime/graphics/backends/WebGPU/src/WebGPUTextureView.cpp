#include "backends/WebGPU/WebGPUTexture.hpp"

#include "backends/WebGPU/WebGPUConverters.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::TextureAspect convertAspect(Graphics::Texture::View::Aspect aspect) {
        switch (aspect) {
            case Graphics::Texture::View::Aspect::All: return wgpu::TextureAspect::All;
            case Graphics::Texture::View::Aspect::Stencil: return wgpu::TextureAspect::StencilOnly;
            case Graphics::Texture::View::Aspect::Depth: return wgpu::TextureAspect::DepthOnly;
            default: throw std::runtime_error("Unexpected texture aspect");
        }
    }

    Graphics::Texture::View::Ref WebGPU::Texture::View::Builder::Build(Graphics::Texture::View::Builder& builder) {
        auto textureHandle = dynamic_cast<WebGPU::Texture*>(builder.getTexture().get())->GetHandle();
        wgpu::TextureViewDescriptor desc = wgpu::Default;
        std::string labelOrDefault = builder.getLabel().empty() ? builder.getTexture()->GetLabel() + " View" : builder.getLabel();
        desc.label = wgpu::StringView(labelOrDefault);
        desc.format = textureHandle.getFormat();
        desc.dimension = builder.getDimension() == Texture::View::Dimension::SameAsTexture ? convertTextureToViewDimension(textureHandle.getDimension()) : convertTextureViewDimension(builder.getDimension());
        desc.baseMipLevel = builder.getBaseMipLevel();
        desc.mipLevelCount = builder.getMipLevelCount() == 0 ? textureHandle.getMipLevelCount() : builder.getMipLevelCount();
        desc.baseArrayLayer = builder.getBaseArrayLayer();
        desc.arrayLayerCount = builder.getArrayLayerCount() == 0 ? textureHandle.getDepthOrArrayLayers() : builder.getArrayLayerCount();
        desc.aspect = convertAspect(builder.getAspect());
        desc.usage = textureHandle.getUsage();
        return std::make_shared<View>(textureHandle.createView(desc), labelOrDefault);
    }

    WebGPU::Texture::View::View(wgpu::TextureView textureView, std::string label) :
        textureView(textureView), label(std::move(label)) { }
    WebGPU::Texture::View::~View() {
        textureView.release();
    }

    void WebGPU::Texture::View::SetLabel(const std::string &newLabel) {
        label = newLabel;
        textureView.setLabel(wgpu::StringView(label));
    }

    wgpu::TextureView WebGPU::Texture::View::GetHandle() { return textureView; }
    const std::string &WebGPU::Texture::View::GetLabel() { return label; }
}
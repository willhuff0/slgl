#include "GFX/WebGPUTexture.hpp"

#include "WebGPUConverters.hpp"

namespace SLGL::Graphics {
    static wgpu::TextureAspect convertAspect(Graphics::Texture::View::Aspect aspect) {
        switch (aspect) {
            case Graphics::Texture::View::Aspect::All: return wgpu::TextureAspect::All;
            case Graphics::Texture::View::Aspect::Stencil: return wgpu::TextureAspect::StencilOnly;
            case Graphics::Texture::View::Aspect::Depth: return wgpu::TextureAspect::DepthOnly;
            default: throw std::runtime_error("Unexpected texture aspect");
        }
    }

    Graphics::Texture::View::Ref WebGPU::Texture::View::Builder::Build(Graphics::Texture::View::Builder &builder) {
        wgpu::TextureViewDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.format = dynamic_cast<WebGPU::Texture*>(builder.getTexture().get())->GetHandle().getFormat();
        desc.dimension = convertTextureViewDimension(builder.getDimension());
        desc.baseMipLevel = builder.getBaseMipLevel();
        desc.mipLevelCount = builder.getMipLevelCount();
        desc.baseArrayLayer = builder.getBaseArrayLayer();
        desc.arrayLayerCount = builder.getArrayLayerCount();
        desc.aspect = convertAspect(builder.getAspect());
        desc.usage = dynamic_cast<WebGPU::Texture*>(builder.getTexture().get())->GetHandle().getUsage();
        return std::make_shared<View>(dynamic_cast<WebGPU::Texture*>(builder.getTexture().get())->GetHandle().createView(desc),
                                      builder.getLabel());
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
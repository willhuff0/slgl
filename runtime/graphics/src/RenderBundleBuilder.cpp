#include <slgl/runtime/graphics/RenderBundle.hpp>

#include <slgl/runtime/graphics/RenderBundleEncoder.hpp>

#include <utility>

namespace SLGL::Graphics {
    RenderBundle::Builder::Builder(RenderBundle::Builder::Impl* impl) : impl(impl) { }

    RenderBundle::Builder& RenderBundle::Builder::SetLabel(const std::string& newLabel) { label = newLabel; return *this; }
    RenderBundle::Builder& RenderBundle::Builder::SetColorFormats(std::vector<std::pair<Texture::Format, Texture::ColorSpace>> newColorFormats) { colorFormats = std::move(newColorFormats); return * this; }
    RenderBundle::Builder& RenderBundle::Builder::SetDepthStencilFormat(std::optional<Texture::Format> newDepthStencilFormat) { depthStencilFormat = newDepthStencilFormat; return * this; }
    RenderBundle::Builder& RenderBundle::Builder::SetSampleCount(uint32_t newSampleCount) { sampleCount = newSampleCount; return * this; }
    RenderBundle::Builder& RenderBundle::Builder::SetDepthReadOnly(bool newDepthReadOnly) { depthReadOnly = newDepthReadOnly; return * this; }
    RenderBundle::Builder& RenderBundle::Builder::SetStencilReadOnly(bool newStencilReadOnly) { stencilReadOnly = newStencilReadOnly; return * this; }

    const std::string& RenderBundle::Builder::getLabel() const { return label; }
    const std::vector<std::pair<Texture::Format, Texture::ColorSpace>>& RenderBundle::Builder::getColorFormats() const { return colorFormats; }
    const std::optional<Texture::Format>& RenderBundle::Builder::getDepthStencilFormat() const { return depthStencilFormat; }
    uint32_t RenderBundle::Builder::getSampleCount() const { return sampleCount; }
    bool RenderBundle::Builder::isDepthReadOnly() const { return depthReadOnly; }
    bool RenderBundle::Builder::isStencilReadOnly() const { return stencilReadOnly; }

    RenderBundle::Ref RenderBundle::Builder::BuildAndEncode(const std::function<void(Graphics::RenderBundleEncoder*)>& func) { return impl->BuildAndEncode(*this, func); }
}
#include <slgl/runtime/graphics/Texture.hpp>

#include <utility>

namespace SLGL::Graphics {
    Texture::Builder::Builder(Impl *impl) : impl(impl) { }

    Texture::Builder& Texture::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    Texture::Builder& Texture::Builder::SetUsage(Usage::Flags newUsage) { usage = newUsage; return *this; }
    Texture::Builder& Texture::Builder::SetSize(glm::ivec2 newSize) { size = newSize; return *this; }
    Texture::Builder& Texture::Builder::SetDimension(Dimension newDimension) { dimension = newDimension; return *this; }
    Texture::Builder& Texture::Builder::SetFormat(Format newFormat) { format = newFormat; return *this; }
    Texture::Builder& Texture::Builder::SetColorSpace(ColorSpace newColorSpace) { colorSpace = newColorSpace; return *this; }
    Texture::Builder& Texture::Builder::SetMipLevels(uint32_t newMipLevels) { mipLevels = newMipLevels; return *this; }
    Texture::Builder& Texture::Builder::SetSamples(uint32_t newSamples) { samples = newSamples; return *this; }

    const std::string &Texture::Builder::getLabel() const { return label; }
    const Texture::Usage::Flags &Texture::Builder::getUsage() const { return usage; }
    const glm::ivec2 &Texture::Builder::getSize() const { return size; }
    Texture::Dimension Texture::Builder::getDimension() const { return dimension; }
    Texture::Format Texture::Builder::getFormat() const { return format; }
    Texture::ColorSpace Texture::Builder::getColorSpace() const { return colorSpace; }
    uint32_t Texture::Builder::getMipLevels() const { return mipLevels; }
    uint32_t Texture::Builder::getSamples() const { return samples; }

    Texture::Ref Texture::Builder::Build() { return impl->Build(*this); }

    // View
    Texture::View::Builder::Builder(Impl* impl, Texture::Ref texture) : impl(impl), texture(std::move(texture)) { }

    Texture::View::Builder& Texture::View::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetDimension(Dimension newDimension) { dimension = newDimension; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetAspect(Aspect newAspect) { aspect = newAspect; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetBaseMipLevel(uint32_t newBaseMipLevel) { baseMipLevel = newBaseMipLevel; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetMipLevelCount(uint32_t newMipLevelCount) { mipLevelCount = newMipLevelCount; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetBaseArrayLayer(uint32_t newBaseArrayLayer) { baseArrayLayer = newBaseArrayLayer; return *this; }
    Texture::View::Builder& Texture::View::Builder::SetArrayLayerCount(uint32_t newArrayLayerCount) { arrayLayerCount = newArrayLayerCount; return *this; }

    const std::string &Texture::View::Builder::getLabel() const { return label; }
    Texture::Ref Texture::View::Builder::getTexture() const { return texture; }
    Texture::View::Dimension Texture::View::Builder::getDimension() const { return dimension; }
    uint32_t Texture::View::Builder::getBaseMipLevel() const { return baseMipLevel; }
    uint32_t Texture::View::Builder::getMipLevelCount() const { return mipLevelCount; }
    uint32_t Texture::View::Builder::getBaseArrayLayer() const { return baseArrayLayer; }
    uint32_t Texture::View::Builder::getArrayLayerCount() const { return arrayLayerCount; }
    Texture::View::Aspect Texture::View::Builder::getAspect() const { return aspect; }

    Texture::View::Ref Texture::View::Builder::Build() { return impl->Build(*this); }
}
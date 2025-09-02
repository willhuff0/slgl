#include <slgl/runtime/graphics/Sampler.hpp>

namespace SLGL::Graphics {
    Sampler::Builder::Builder(Sampler::Builder::Impl *impl) : impl(impl) { }

    Sampler::Builder& Sampler::Builder::SetLabel(const std::string &newLabel) { label = newLabel; return *this; }
    Sampler::Builder& Sampler::Builder::SetAddressModeU(AddressMode newAddressModeU) { addressModeU = newAddressModeU; return *this; }
    Sampler::Builder& Sampler::Builder::SetAddressModeV(AddressMode newAddressModeV) { addressModeV = newAddressModeV; return *this; }
    Sampler::Builder& Sampler::Builder::SetAddressModeW(AddressMode newAddressModeW) { addressModeW = newAddressModeW; return *this; }
    Sampler::Builder& Sampler::Builder::SetMagFilter(FilterMode newMagFilter) { magFilter = newMagFilter; return *this; }
    Sampler::Builder& Sampler::Builder::SetMinFilter(FilterMode newMinFilter) { minFilter = newMinFilter; return *this; }
    Sampler::Builder& Sampler::Builder::SetMipmapFilter(FilterMode newMipmapFilter) { mipmapFilter = newMipmapFilter; return *this; }
    Sampler::Builder& Sampler::Builder::SetCompareMode(CompareMode newCompareMode) { compareMode = newCompareMode; return *this; }
    Sampler::Builder& Sampler::Builder::SetMinLod(float newMinLod) { minLod = newMinLod; return *this; }
    Sampler::Builder& Sampler::Builder::SetMaxLod(float newMaxLod) { maxLod = newMaxLod; return *this; }
    Sampler::Builder &Sampler::Builder::SetMaxAnisotropy(uint16_t newMaxAnisotropy) { maxAnisotropy = newMaxAnisotropy; return *this; }

    const std::string &Sampler::Builder::getLabel() const { return label; }
    Sampler::AddressMode Sampler::Builder::getAddressModeU() const { return addressModeU; }
    Sampler::AddressMode Sampler::Builder::getAddressModeV() const { return addressModeV; }
    Sampler::AddressMode Sampler::Builder::getAddressModeW() const { return addressModeW; }
    Sampler::FilterMode Sampler::Builder::getMagFilter() const { return magFilter; }
    Sampler::FilterMode Sampler::Builder::getMinFilter() const { return minFilter; }
    Sampler::FilterMode Sampler::Builder::getMipmapFilter() const { return mipmapFilter; }
    Sampler::CompareMode Sampler::Builder::getCompareMode() const { return compareMode; }
    float Sampler::Builder::getMinLod() const { return minLod; }
    float Sampler::Builder::getMaxLod() const { return maxLod; }
    uint16_t Sampler::Builder::getMaxAnisotropy() const { return maxAnisotropy; }

    Sampler::Ref Sampler::Builder::Build() { return impl->Build(*this); }
}
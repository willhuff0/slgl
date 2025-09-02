#include "GFX/WebGPUSampler.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>

namespace SLGL::Graphics {
    static wgpu::AddressMode convertAddressMode(Graphics::Sampler::AddressMode addressMode) {
        switch (addressMode) {
            case Graphics::Sampler::AddressMode::Clamp: return wgpu::AddressMode::ClampToEdge;
            case Graphics::Sampler::AddressMode::Repeat: return wgpu::AddressMode::Repeat;
            case Graphics::Sampler::AddressMode::Mirror: return wgpu::AddressMode::MirrorRepeat;
            default: throw std::runtime_error("Unexpected AddressMode");
        }
    }

    static wgpu::FilterMode convertFilterMode(Graphics::Sampler::FilterMode filterMode) {
        switch (filterMode) {
            case Graphics::Sampler::FilterMode::Nearest: return wgpu::FilterMode::Nearest;
            case Graphics::Sampler::FilterMode::Linear: return wgpu::FilterMode::Linear;
            default: throw std::runtime_error("Unexpected FilterMode");
        }
    }

    static wgpu::MipmapFilterMode convertMipmapFilterMode(Graphics::Sampler::FilterMode filterMode) {
        switch (filterMode) {
            case Graphics::Sampler::FilterMode::Nearest: return wgpu::MipmapFilterMode::Nearest;
            case Graphics::Sampler::FilterMode::Linear: return wgpu::MipmapFilterMode::Linear;
            default: throw std::runtime_error("Unexpected MipmapFilterMode");
        }
    }

    static wgpu::CompareFunction convertCompareFunction(Graphics::Sampler::CompareMode compareMode) {
        switch (compareMode) {
            case Graphics::Sampler::CompareMode::Undefined: return wgpu::CompareFunction::Undefined;
            case Graphics::Sampler::CompareMode::Never: return wgpu::CompareFunction::Never;
            case Graphics::Sampler::CompareMode::Less: return wgpu::CompareFunction::Less;
            case Graphics::Sampler::CompareMode::LessEqual: return wgpu::CompareFunction::LessEqual;
            case Graphics::Sampler::CompareMode::Greater: return wgpu::CompareFunction::Greater;
            case Graphics::Sampler::CompareMode::GreaterEqual: return wgpu::CompareFunction::GreaterEqual;
            case Graphics::Sampler::CompareMode::Equal: return wgpu::CompareFunction::Equal;
            case Graphics::Sampler::CompareMode::NotEqual: return wgpu::CompareFunction::NotEqual;
            case Graphics::Sampler::CompareMode::Always: return wgpu::CompareFunction::Always;
            default: throw std::runtime_error("Unexpected CompareFunction");
        }
    }

    WebGPU::Sampler::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::Sampler::Ref WebGPU::Sampler::Builder::Build(Graphics::Sampler::Builder &builder) {
        wgpu::SamplerDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.addressModeU = convertAddressMode(builder.getAddressModeU());
        desc.addressModeV = convertAddressMode(builder.getAddressModeV());
        desc.addressModeW = convertAddressMode(builder.getAddressModeW());
        desc.magFilter = convertFilterMode(builder.getMagFilter());
        desc.minFilter = convertFilterMode(builder.getMinFilter());
        desc.mipmapFilter = convertMipmapFilterMode(builder.getMipmapFilter());
        desc.compare = convertCompareFunction(builder.getCompareMode());
        desc.lodMinClamp = builder.getMinLod();
        desc.lodMaxClamp = builder.getMaxLod();
        desc.maxAnisotropy = builder.getMaxAnisotropy();
        return std::make_shared<Sampler>(ctx.device.createSampler(desc), builder.getLabel());
    }

    WebGPU::Sampler::~Sampler() {
        sampler.release();
    }

    WebGPU::Sampler::Sampler(wgpu::Sampler sampler, std::string label) : sampler(sampler), label(std::move(label)) {}

    void WebGPU::Sampler::SetLabel(const std::string &newLabel) {
        label = newLabel;
        sampler.setLabel(wgpu::StringView(label));
    }

    wgpu::Sampler WebGPU::Sampler::GetHandle() { return sampler; }
    const std::string &WebGPU::Sampler::GetLabel() { return label; }
}
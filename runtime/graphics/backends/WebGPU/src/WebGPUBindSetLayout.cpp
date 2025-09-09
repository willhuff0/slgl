#include "backends/WebGPU/WebGPUBindSet.hpp"

#include "backends/WebGPU/WebGPUConverters.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::ShaderStage convertVisibility(Graphics::BindSet::Layout::Entry::Visibility::Flags flags) {
        const auto& flagBits = flags.GetBitset();
        WGPUShaderStage stage = wgpu::ShaderStage::None;
        if (flagBits.test(0)) stage |= wgpu::ShaderStage::Vertex;
        if (flagBits.test(1)) stage |= wgpu::ShaderStage::Fragment;
        if (flagBits.test(2)) stage |= wgpu::ShaderStage::Compute;
        return stage;
    }

    static wgpu::BufferBindingType convertBufferType(Graphics::BindSet::Layout::Entry::Buffer::Type type) {
        switch (type) {
            case Graphics::BindSet::Layout::Entry::Buffer::Type::Undefined: throw std::runtime_error("Buffer type is undefined");
            case Graphics::BindSet::Layout::Entry::Buffer::Type::Uniform: return wgpu::BufferBindingType::Uniform;
            case Graphics::BindSet::Layout::Entry::Buffer::Type::Storage: return wgpu::BufferBindingType::Storage;
            case Graphics::BindSet::Layout::Entry::Buffer::Type::ReadOnlyStorage: return wgpu::BufferBindingType::ReadOnlyStorage;
            default: throw std::runtime_error("Unexpected buffer type");
        }
    }

    static wgpu::SamplerBindingType convertSamplerType(Graphics::BindSet::Layout::Entry::Sampler::Type type) {
        switch (type) {
            case Graphics::BindSet::Layout::Entry::Sampler::Type::Filtering: return wgpu::SamplerBindingType::Filtering;
            case Graphics::BindSet::Layout::Entry::Sampler::Type::NonFiltering: return wgpu::SamplerBindingType::NonFiltering;
            case Graphics::BindSet::Layout::Entry::Sampler::Type::Comparison: return wgpu::SamplerBindingType::Comparison;
            default: throw std::runtime_error("Unexpected sampler type");
        }
    }

    static wgpu::TextureSampleType convertSampleType(Graphics::BindSet::Layout::Entry::Texture::SampleType sampleType) {
        switch (sampleType) {
            case Graphics::BindSet::Layout::Entry::Texture::SampleType::Float: return wgpu::TextureSampleType::Float;
            case Graphics::BindSet::Layout::Entry::Texture::SampleType::UnfilterableFloat: return wgpu::TextureSampleType::UnfilterableFloat;
            case Graphics::BindSet::Layout::Entry::Texture::SampleType::Depth: return wgpu::TextureSampleType::Depth;
            case Graphics::BindSet::Layout::Entry::Texture::SampleType::Sint: return wgpu::TextureSampleType::Sint;
            case Graphics::BindSet::Layout::Entry::Texture::SampleType::Uint: return wgpu::TextureSampleType::Uint;
            default: throw std::runtime_error("Unexpected sampler type");
        }
    }

    static wgpu::StorageTextureAccess convertAccess(Graphics::BindSet::Layout::Entry::StorageTexture::Access access) {
        switch (access) {
            case Graphics::BindSet::Layout::Entry::StorageTexture::Access::WriteOnly: return wgpu::StorageTextureAccess::WriteOnly;
            case Graphics::BindSet::Layout::Entry::StorageTexture::Access::ReadOnly: return wgpu::StorageTextureAccess::ReadOnly;
            case Graphics::BindSet::Layout::Entry::StorageTexture::Access::ReadWrite: return wgpu::StorageTextureAccess::ReadWrite;
            default: throw std::runtime_error("Unexpected storage texture access");
        }
    }

    WebGPU::BindSet::Layout::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::BindSet::Layout::Ref WebGPU::BindSet::Layout::Builder::Build(Graphics::BindSet::Layout::Builder &builder) {
        wgpu::BindGroupLayoutDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.entryCount = builder.getEntries().size();
        std::vector<wgpu::BindGroupLayoutEntry> _entries;
        _entries.reserve(desc.entryCount);
        for(const auto& [binding, entry] : builder.getEntries()) {
            wgpu::BindGroupLayoutEntry result = wgpu::Default;
            result.binding = binding;
            result.visibility = convertVisibility(entry.visibility);
            switch(entry.type) {
                case Graphics::BindSet::Layout::Entry::Type::Undefined: throw std::runtime_error("Binding type is undefined");
                case Graphics::BindSet::Layout::Entry::Type::Buffer: {
                    result.buffer.type = convertBufferType(entry.buffer.type);
                    result.buffer.hasDynamicOffset = entry.buffer.isDynamic;
                    result.buffer.minBindingSize = entry.buffer.minSize;
                    break;
                }
                case Graphics::BindSet::Layout::Entry::Type::Sampler: {
                    result.sampler.type = convertSamplerType(entry.sampler.type);
                    break;
                }
                case Graphics::BindSet::Layout::Entry::Type::Texture: {
                    result.texture.sampleType = convertSampleType(entry.texture.sampleType);
                    result.texture.viewDimension = convertTextureViewDimension(entry.texture.viewDimension);
                    result.texture.multisampled = entry.texture.multisample;
                    break;
                }
                case Graphics::BindSet::Layout::Entry::Type::StorageTexture: {
                    result.storageTexture.access = convertAccess(entry.storageTexture.access);
                    result.storageTexture.format = convertTextureFormat(entry.storageTexture.format,
                                                                        entry.storageTexture.colorSpace);
                    result.storageTexture.viewDimension = convertTextureViewDimension(entry.texture.viewDimension);
                    break;
                }
            }
            _entries.push_back(result);
        }
        desc.entries = _entries.data();
        return std::make_shared<Layout>(ctx, ctx.device.createBindGroupLayout(desc), builder.getLabel(), builder.getEntries());
    }


    void WebGPU::BindSet::Layout::SetLabel(const std::string &newLabel) {
        label = newLabel;
        bindGroupLayout.setLabel(wgpu::StringView(label));
    }

    WebGPU::BindSet::Layout::Layout(WebGPU::Context &ctx, wgpu::BindGroupLayout bindGroupLayout, std::string label,
                                    std::unordered_map<uint32_t, Entry> entries) : ctx(ctx),
                                    bindGroupLayout(bindGroupLayout), label(std::move(label)), entries(std::move(entries)) { }
    WebGPU::BindSet::Layout::~Layout() {
        bindGroupLayout.release();
    }

    Graphics::BindSet::Builder WebGPU::BindSet::Layout::Instantiate() {
        static WebGPU::BindSet::Builder bindSetBuilderImpl(ctx);
        return Graphics::BindSet::Builder(&bindSetBuilderImpl, shared_from_this());
    }

    wgpu::BindGroupLayout WebGPU::BindSet::Layout::GetHandle() { return bindGroupLayout; }
    const std::string &WebGPU::BindSet::Layout::GetLabel() { return label; }
    const std::unordered_map<uint32_t, Graphics::BindSet::Layout::Entry>& WebGPU::BindSet::Layout::GetEntries() { return entries; }
}
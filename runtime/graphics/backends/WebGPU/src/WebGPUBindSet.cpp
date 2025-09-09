#include "backends/WebGPU/WebGPUBindSet.hpp"

#include "backends/WebGPU/WebGPUBuffer.hpp"
#include "backends/WebGPU/WebGPUSampler.hpp"
#include "backends/WebGPU/WebGPUTexture.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    WebGPU::BindSet::Builder::Builder(WebGPU::Context &ctx) : ctx(ctx) { }
    Graphics::BindSet::Ref WebGPU::BindSet::Builder::Build(Graphics::BindSet::Builder &builder) {
        wgpu::BindGroupDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.layout = dynamic_cast<WebGPU::BindSet::Layout*>(builder.getLayout().get())->GetHandle();
        desc.entryCount = builder.getEntries().size();
        std::vector<wgpu::BindGroupEntry> entries;
        entries.reserve(desc.entryCount);
        for(const auto& [binding, entry] : builder.getEntries()) {
            wgpu::BindGroupEntry result = wgpu::Default;
            result.binding = binding;
            switch(builder.getLayout()->GetEntries().at(binding).type) {
                case Graphics::BindSet::Layout::Entry::Type::Undefined: throw std::runtime_error("Binding type is undefined");
                case Graphics::BindSet::Layout::Entry::Type::Buffer: {
                    auto buffer = std::get<Graphics::BindSet::Entry::Buffer>(entry.value);
                    result.buffer = dynamic_cast<WebGPU::Buffer*>(buffer.buffer.get())->GetHandle();
                    result.offset = buffer.offset;
                    result.size = buffer.size;
                    break;
                }
                case Graphics::BindSet::Layout::Entry::Type::Sampler: {
                    auto sampler = std::get<Graphics::BindSet::Entry::Sampler>(entry.value);
                    result.sampler = dynamic_cast<WebGPU::Sampler*>(sampler.sampler.get())->GetHandle();
                    break;
                }
                case Graphics::BindSet::Layout::Entry::Type::Texture:
                case Graphics::BindSet::Layout::Entry::Type::StorageTexture: {
                    auto textureView = std::get<Graphics::BindSet::Entry::TextureView>(entry.value);
                    result.textureView = dynamic_cast<WebGPU::Texture::View*>(textureView.textureView.get())->GetHandle();
                    break;
                }
            }
            entries.push_back(result);
        }
        desc.entries = entries.data();
        return std::make_shared<BindSet>(ctx.device.createBindGroup(desc), builder.getLabel(), builder.getLayout());
    }

    WebGPU::BindSet::BindSet(wgpu::BindGroup bindGroup, std::string label, Graphics::BindSet::Layout::Ref layout)
        : bindGroup(bindGroup), label(std::move(label)), layout(std::move(layout)) { }
    WebGPU::BindSet::~BindSet() {
        bindGroup.release();
    }

    void WebGPU::BindSet::SetLabel(const std::string &newLabel) {
        label = newLabel;
        bindGroup.setLabel(wgpu::StringView(label));
    }

    wgpu::BindGroup WebGPU::BindSet::GetHandle() { return bindGroup; }
    const std::string &WebGPU::BindSet::GetLabel() { return label; }
    Graphics::BindSet::Layout::Ref WebGPU::BindSet::GetLayout() { return layout; }
}
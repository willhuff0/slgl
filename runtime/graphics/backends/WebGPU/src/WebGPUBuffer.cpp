#include "backends/WebGPU/WebGPUBuffer.hpp"

#include <webgpu/webgpu.hpp>

#include <memory>

namespace SLGL::Graphics {
    static wgpu::BufferUsage convertUsage(Graphics::Buffer::Usage::Flags flags) {
        const auto& flagBits = flags.GetBitset();
        WGPUBufferUsage usage = wgpu::BufferUsage::None;
        if (flagBits.test(0)) usage |= wgpu::BufferUsage::CopyDst;
        if (flagBits.test(1)) usage |= wgpu::BufferUsage::CopySrc;
        if (flagBits.test(2)) usage |= wgpu::BufferUsage::Index;
        if (flagBits.test(3)) usage |= wgpu::BufferUsage::Vertex;
        if (flagBits.test(4)) usage |= wgpu::BufferUsage::Uniform;
        if (flagBits.test(5)) usage |= wgpu::BufferUsage::Storage;
        return usage;
    }

    WebGPU::Buffer::Builder::Builder(WebGPU::Context& ctx) : ctx(ctx) { }
    Graphics::Buffer::Ref WebGPU::Buffer::Builder::Build(Graphics::Buffer::Builder& builder) {
        wgpu::BufferDescriptor desc = wgpu::Default;
        desc.label = wgpu::StringView(builder.getLabel());
        desc.usage = convertUsage(builder.getUsage());
        desc.size = builder.getSize();
        return std::make_shared<Buffer>(ctx.device.createBuffer(desc), builder.getLabel());
    }

    WebGPU::Buffer::Buffer(wgpu::Buffer buffer, std::string label) : buffer(buffer), label(std::move(label)) { }
    WebGPU::Buffer::~Buffer() {
        buffer.release();
    }

    void WebGPU::Buffer::SetLabel(const std::string& newLabel) {
        label = newLabel;
        buffer.setLabel(wgpu::StringView(label));
    }

    wgpu::Buffer WebGPU::Buffer::GetHandle() { return buffer; }
    const std::string &WebGPU::Buffer::GetLabel() { return label; }
    uint64_t WebGPU::Buffer::GetSize() { return buffer.getSize(); }

    void WebGPU::Buffer::Write(Graphics::Queue* queue, void* src, uint64_t offset, uint64_t size) {
        ((wgpu::Queue)(WGPUQueue)queue).writeBuffer(buffer, offset, src, size);
    }
    void WebGPU::Buffer::Write(Graphics::Queue* queue, void* src) {
        Write(queue, src, 0, GetSize());
    }
}
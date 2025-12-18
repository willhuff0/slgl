#include "backends/WebGPU/WebGPURenderBundleEncoder.hpp"

#include "backends/WebGPU/WebGPUShaderRenderPipeline.hpp"
#include "backends/WebGPU/WebGPUBindSet.hpp"
#include "backends/WebGPU/WebGPUBuffer.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::IndexFormat convertIndexFormat(Graphics::RenderBundleEncoder::IndexFormat format) {
        switch (format) {
            case Graphics::RenderBundleEncoder::IndexFormat::Uint16: return wgpu::IndexFormat::Uint16;
            case Graphics::RenderBundleEncoder::IndexFormat::Uint32: return wgpu::IndexFormat::Uint32;
            default: throw std::runtime_error("Unexpected index format");
        }
    }

    WebGPU::RenderBundleEncoder::RenderBundleEncoder(wgpu::RenderBundleEncoder& encoder) : encoder(encoder) { }

    void WebGPU::RenderBundleEncoder::InsertDebugMarker(const std::string& label) {
        encoder.insertDebugMarker(wgpu::StringView(label));
    }
    void WebGPU::RenderBundleEncoder::PushDebugScope(const std::string& label) {
        encoder.pushDebugGroup(wgpu::StringView(label));
    }
    void WebGPU::RenderBundleEncoder::PopDebugScope() {
        encoder.popDebugGroup();
    }

    void WebGPU::RenderBundleEncoder::SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) {
        encoder.setPipeline(dynamic_cast<ShaderPipeline::Render*>(pipeline.get())->GetHandle());
    }

    void WebGPU::RenderBundleEncoder::SetImmediateData(uint32_t offset, void* data, uint64_t size) {
        encoder.setImmediateData(offset, data, size);
    }

    void WebGPU::RenderBundleEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet, const std::vector<uint32_t>& dynamicOffset) {
        encoder.setBindGroup(index, dynamic_cast<BindSet*>(bindSet.get())->GetHandle(), dynamicOffset);
    }
    void WebGPU::RenderBundleEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet) {
        SetBindSet(index, bindSet, {});
    }

    void WebGPU::RenderBundleEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderBundleEncoder::IndexFormat format, uint64_t offset, uint64_t size) {
        encoder.setIndexBuffer(dynamic_cast<Buffer*>(buffer.get())->GetHandle(), convertIndexFormat(format), offset, size);
    }
    void WebGPU::RenderBundleEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderBundleEncoder::IndexFormat format, uint64_t offset) {
        SetIndexBuffer(buffer, format, offset, buffer->GetSize());
    }
    void WebGPU::RenderBundleEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderBundleEncoder::IndexFormat format) {
        SetIndexBuffer(buffer, format, 0);
    }

    void WebGPU::RenderBundleEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset, uint64_t size) {
        encoder.setVertexBuffer(slot, dynamic_cast<Buffer*>(buffer.get())->GetHandle(), offset, size);
    }
    void WebGPU::RenderBundleEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset) {
        SetVertexBuffer(slot, buffer, offset, buffer->GetSize());
    }
    void WebGPU::RenderBundleEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer) {
        SetVertexBuffer(slot, buffer, 0);
    }

    void WebGPU::RenderBundleEncoder::Draw(uint32_t vertexCount, uint32_t vertexOffset) {
        encoder.draw(vertexCount, 1, vertexOffset, 0);
    }
    void WebGPU::RenderBundleEncoder::Draw(uint32_t vertexCount) {
        Draw(vertexCount, 0);
    }
    void WebGPU::RenderBundleEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset) {
        encoder.draw(vertexCount, instanceCount, vertexOffset, instanceOffset);
    }
    void WebGPU::RenderBundleEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount) {
        DrawInstances(vertexCount, instanceCount, 0, 0);
    }
    void WebGPU::RenderBundleEncoder::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) {
        encoder.drawIndexed(indexCount, 1, vertexOffset, 0, 0);
    }
    void WebGPU::RenderBundleEncoder::DrawIndexed(uint32_t indexCount) {
        DrawIndexed(indexCount, 0, 0);
    }
    void WebGPU::RenderBundleEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, int32_t vertexBase, uint32_t instanceOffset) {
        encoder.drawIndexed(indexCount, instanceCount, indexOffset, vertexBase, instanceOffset);
    }
    void WebGPU::RenderBundleEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount) {
        DrawInstancesIndexed(indexCount, instanceCount, 0, 0, 0);
    }
}
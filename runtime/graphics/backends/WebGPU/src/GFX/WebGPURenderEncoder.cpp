#include "GFX/WebGPURenderEncoder.hpp"

#include "GFX/WebGPUShaderRenderPipeline.hpp"
#include "GFX/WebGPUBindSet.hpp"
#include "GFX/WebGPUBuffer.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::IndexFormat convertIndexFormat(Graphics::RenderEncoder::IndexFormat format) {
        switch (format) {
            case Graphics::RenderEncoder::IndexFormat::Uint16: return wgpu::IndexFormat::Uint16;
            case Graphics::RenderEncoder::IndexFormat::Uint32: return wgpu::IndexFormat::Uint32;
            default: throw std::runtime_error("Unexpected index format");
        }
    }

    WebGPU::RenderEncoder::RenderEncoder(wgpu::RenderPassEncoder &renderPass) : renderPass(renderPass) { }

    void WebGPU::RenderEncoder::SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) {
        renderPass.setPipeline(dynamic_cast<ShaderPipeline::Render*>(pipeline.get())->GetHandle());
    }

    void WebGPU::RenderEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet, const std::vector<uint32_t> &dynamicOffset) {
        renderPass.setBindGroup(index, dynamic_cast<BindSet*>(bindSet.get())->GetHandle(), dynamicOffset);
    }
    void WebGPU::RenderEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet) {
        SetBindSet(index, bindSet, {});
    }

    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format, uint64_t offset, uint64_t size) {
        renderPass.setIndexBuffer(dynamic_cast<Buffer*>(buffer.get())->GetHandle(), convertIndexFormat(format), offset, size);
    }
    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format, uint64_t offset) {
        SetIndexBuffer(buffer, format, offset, buffer->GetSize());
    }
    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format) {
        SetIndexBuffer(buffer, format, 0);
    }

    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset, uint64_t size) {
        renderPass.setVertexBuffer(slot, dynamic_cast<Buffer*>(buffer.get())->GetHandle(), offset, size);
    }
    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset) {
        SetVertexBuffer(slot, buffer, offset, buffer->GetSize());
    }
    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer) {
        SetVertexBuffer(slot, buffer, 0);
    }

    void WebGPU::RenderEncoder::Draw(uint32_t vertexCount, uint32_t vertexOffset) {
        renderPass.draw(vertexCount, 1, vertexOffset, 0);
    }
    void WebGPU::RenderEncoder::Draw(uint32_t vertexCount) {
        Draw(vertexCount, 0);
    }
    void WebGPU::RenderEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset) {
        renderPass.draw(vertexCount, instanceCount, vertexOffset, instanceOffset);
    }
    void WebGPU::RenderEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount) {
        DrawInstances(vertexCount, instanceCount, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) {
        renderPass.drawIndexed(indexCount, 1, vertexOffset, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawIndexed(uint32_t indexCount) {
        DrawIndexed(indexCount, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, int32_t vertexBase, uint32_t instanceOffset) {
        renderPass.drawIndexed(indexCount, instanceCount, indexOffset, vertexBase, instanceOffset);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount) {
        DrawInstancesIndexed(indexCount, instanceCount, 0, 0, 0);
    }
}
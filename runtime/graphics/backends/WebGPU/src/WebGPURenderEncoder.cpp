#include "backends/WebGPU/WebGPURenderEncoder.hpp"

#include "backends/WebGPU/WebGPUShaderRenderPipeline.hpp"
#include "backends/WebGPU/WebGPUBindSet.hpp"
#include "backends/WebGPU/WebGPUBuffer.hpp"
#include "backends/WebGPU/WebGPURenderBundle.hpp"

#include <webgpu/webgpu.hpp>

namespace SLGL::Graphics {
    static wgpu::IndexFormat convertIndexFormat(Graphics::RenderEncoder::IndexFormat format) {
        switch (format) {
            case Graphics::RenderEncoder::IndexFormat::Uint16: return wgpu::IndexFormat::Uint16;
            case Graphics::RenderEncoder::IndexFormat::Uint32: return wgpu::IndexFormat::Uint32;
            default: throw std::runtime_error("Unexpected index format");
        }
    }

    WebGPU::RenderEncoder::RenderEncoder(wgpu::RenderPassEncoder& encoder) : encoder(encoder) { }

    void WebGPU::RenderEncoder::InsertDebugMarker(const std::string& label) {
        encoder.insertDebugMarker(wgpu::StringView(label));
    }
    void WebGPU::RenderEncoder::PushDebugScope(const std::string& label) {
        encoder.pushDebugGroup(wgpu::StringView(label));
    }
    void WebGPU::RenderEncoder::PopDebugScope() {
        encoder.popDebugGroup();
    }

    void WebGPU::RenderEncoder::SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) {
        encoder.setPipeline(dynamic_cast<ShaderPipeline::Render*>(pipeline.get())->GetHandle());
    }

    void WebGPU::RenderEncoder::SetViewport(glm::vec2 offset, glm::vec2 size, float minDepth, float maxDepth) {
        encoder.setViewport(offset.x, offset.y, size.x, size.y, minDepth, maxDepth);
    }

    void WebGPU::RenderEncoder::SetScissor(glm::vec<2, uint32_t, glm::defaultp> offset, glm::vec<2, uint32_t, glm::defaultp> size) {
        encoder.setScissorRect(offset.x, offset.y, size.x, size.y);
    }
    void WebGPU::RenderEncoder::SetStencilReference(uint32_t reference) {
        encoder.setStencilReference(reference);
    }
    void WebGPU::RenderEncoder::SetBlendConstant(glm::vec4 color) {
        encoder.setBlendConstant({ (double)color.r, (double)color.g, (double)color.b, (double)color.a });
    }
    void WebGPU::RenderEncoder::SetImmediateData(uint32_t offset, void* data, uint64_t size) {
        encoder.setImmediateData(offset, data, size);
    }

    void WebGPU::RenderEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet, const std::vector<uint32_t>& dynamicOffset) {
        encoder.setBindGroup(index, dynamic_cast<BindSet*>(bindSet.get())->GetHandle(), dynamicOffset);
    }
    void WebGPU::RenderEncoder::SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet) {
        SetBindSet(index, bindSet, {});
    }

    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format, uint64_t offset, uint64_t size) {
        encoder.setIndexBuffer(dynamic_cast<Buffer*>(buffer.get())->GetHandle(), convertIndexFormat(format), offset, size);
    }
    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format, uint64_t offset) {
        SetIndexBuffer(buffer, format, offset, buffer->GetSize());
    }
    void WebGPU::RenderEncoder::SetIndexBuffer(Graphics::Buffer::Ref buffer, Graphics::RenderEncoder::IndexFormat format) {
        SetIndexBuffer(buffer, format, 0);
    }

    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset, uint64_t size) {
        encoder.setVertexBuffer(slot, dynamic_cast<Buffer*>(buffer.get())->GetHandle(), offset, size);
    }
    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset) {
        SetVertexBuffer(slot, buffer, offset, buffer->GetSize());
    }
    void WebGPU::RenderEncoder::SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer) {
        SetVertexBuffer(slot, buffer, 0);
    }

    void WebGPU::RenderEncoder::Draw(uint32_t vertexCount, uint32_t vertexOffset) {
        encoder.draw(vertexCount, 1, vertexOffset, 0);
    }
    void WebGPU::RenderEncoder::Draw(uint32_t vertexCount) {
        Draw(vertexCount, 0);
    }
    void WebGPU::RenderEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset) {
        encoder.draw(vertexCount, instanceCount, vertexOffset, instanceOffset);
    }
    void WebGPU::RenderEncoder::DrawInstances(uint32_t vertexCount, uint32_t instanceCount) {
        DrawInstances(vertexCount, instanceCount, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) {
        encoder.drawIndexed(indexCount, 1, vertexOffset, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawIndexed(uint32_t indexCount) {
        DrawIndexed(indexCount, 0, 0);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, int32_t vertexBase, uint32_t instanceOffset) {
        encoder.drawIndexed(indexCount, instanceCount, indexOffset, vertexBase, instanceOffset);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount) {
        DrawInstancesIndexed(indexCount, instanceCount, 0, 0, 0);
    }

    void WebGPU::RenderEncoder::DrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) {
        encoder.drawIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), byteOffset);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndirect(Buffer::Ref argumentBuffer) {
        DrawInstancesIndirect(argumentBuffer, 0);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) {
        encoder.drawIndexedIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), byteOffset);
    }
    void WebGPU::RenderEncoder::DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer) {
        DrawInstancesIndexedIndirect(argumentBuffer, 0);
    }

    void WebGPU::RenderEncoder::MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) {
        encoder.multiDrawIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), byteOffset, drawCount, nullptr, 0);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) {
        MultiDrawInstancesIndirect(argumentBuffer, 0, drawCount);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) {
        encoder.multiDrawIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), argumentBufferByteOffset, maxDrawCount, dynamic_cast<Buffer*>(drawCountBuffer.get())->GetHandle(), drawCountBufferByteOffset);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) {
        MultiDrawInstancesIndirect(argumentBuffer, 0, maxDrawCount, drawCountBuffer, 0);
    }

    void WebGPU::RenderEncoder::MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) {
        encoder.multiDrawIndexedIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), byteOffset, drawCount, nullptr, 0);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) {
        MultiDrawInstancesIndexedIndirect(argumentBuffer, 0, drawCount);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) {
        encoder.multiDrawIndirect(dynamic_cast<Buffer*>(argumentBuffer.get())->GetHandle(), argumentBufferByteOffset, maxDrawCount, dynamic_cast<Buffer*>(drawCountBuffer.get())->GetHandle(), drawCountBufferByteOffset);
    }
    void WebGPU::RenderEncoder::MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) {
        MultiDrawInstancesIndexedIndirect(argumentBuffer, 0, maxDrawCount, drawCountBuffer, 0);
    }

    void WebGPU::RenderEncoder::BeginOcclusionQuery(uint32_t index) {
        encoder.beginOcclusionQuery(index);
    }
    void WebGPU::RenderEncoder::EndOcclusionQuery() {
        encoder.endOcclusionQuery();
    }

    void WebGPU::RenderEncoder::ExecuteRenderBundles(const std::vector<RenderBundle::Ref>& renderBundles) {
        std::vector<WGPURenderBundle> wgpuRenderBundles;
        wgpuRenderBundles.reserve(renderBundles.size());
        for (const auto& renderBundle : renderBundles) {
            wgpuRenderBundles.push_back(dynamic_cast<RenderBundle*>(renderBundle.get())->GetHandle());
        }
        encoder.executeBundles(wgpuRenderBundles);
    }
    void WebGPU::RenderEncoder::ExecuteRenderBundles(RenderBundle::Ref renderBundle) {
        encoder.executeBundles(dynamic_cast<RenderBundle*>(renderBundle.get())->GetHandle());
    }
}
#pragma once

#include <slgl/runtime/graphics/RenderEncoder.hpp>

#include <webgpu/webgpu.hpp>

#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class RenderEncoder : public Graphics::RenderEncoder {
    public:
        explicit RenderEncoder(wgpu::RenderPassEncoder& encoder);

        void InsertDebugMarker(const std::string& label) override;
        void PushDebugScope(const std::string& label) override;
        void PopDebugScope() override;

        void SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) override;

        void SetViewport(glm::vec2 offset, glm::vec2 size, float minDepth, float maxDepth) override;

        void SetScissor(glm::vec<2, uint32_t, glm::defaultp> offset, glm::vec<2, uint32_t, glm::defaultp> size) override;
        void SetStencilReference(uint32_t reference) override;
        void SetBlendConstant(glm::vec4 color) override;
        void SetImmediateData(uint32_t offset, void* data, uint64_t size) override;

        void SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet, const std::vector<uint32_t>& dynamicOffset) override;
        void SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet) override;

        void SetIndexBuffer(Graphics::Buffer::Ref buffer, IndexFormat format, uint64_t offset, uint64_t size) override;
        void SetIndexBuffer(Graphics::Buffer::Ref buffer, IndexFormat format, uint64_t offset) override;
        void SetIndexBuffer(Graphics::Buffer::Ref buffer, IndexFormat format) override;

        void SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset, uint64_t size) override;
        void SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer, uint64_t offset) override;
        void SetVertexBuffer(uint32_t slot, Graphics::Buffer::Ref buffer) override;

        void Draw(uint32_t vertexCount, uint32_t vertexOffset) override;
        void Draw(uint32_t vertexCount) override;
        void DrawInstances(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset) override;
        void DrawInstances(uint32_t vertexCount, uint32_t instanceCount) override;
        void DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) override;
        void DrawIndexed(uint32_t indexCount) override;
        void DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, int32_t vertexBase, uint32_t instanceOffset) override;
        void DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount) override;

        void DrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) override;
        void DrawInstancesIndirect(Buffer::Ref argumentBuffer) override;
        void DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) override;
        void DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer) override;

        void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) override;
        void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) override;
        void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) override;
        void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) override;

        void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) override;
        void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) override;
        void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) override;
        void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) override;

        void BeginOcclusionQuery(uint32_t index) override;
        void EndOcclusionQuery() override;

        void ExecuteRenderBundles(const std::vector<RenderBundle::Ref>& renderBundles) override;
        void ExecuteRenderBundles(RenderBundle::Ref renderBundle) override;

    private:
        wgpu::RenderPassEncoder& encoder;
    };
}

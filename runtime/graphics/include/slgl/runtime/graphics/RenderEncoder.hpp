#pragma once

#include <slgl/common/Math.hpp>

#include "Buffer.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "BindSet.hpp"
#include "ShaderPipeline.hpp"
#include "RenderBundle.hpp"

#include <vector>
#include <cstdint>

namespace SLGL::Graphics {
    class RenderEncoder {
    public:
        enum class IndexFormat : uint8_t {
            Uint16,
            Uint32,
        };

        virtual ~RenderEncoder() = default;

        virtual void InsertDebugMarker(const std::string& label) = 0;
        virtual void PushDebugScope(const std::string& label) = 0;
        virtual void PopDebugScope() = 0;

        virtual void SetPipeline(ShaderPipeline::Render::Ref pipeline) = 0;

        virtual void SetViewport(glm::vec2 offset, glm::vec2 size, float minDepth, float maxDepth) = 0;

        virtual void SetScissor(glm::vec<2, uint32_t, glm::defaultp> offset, glm::vec<2, uint32_t, glm::defaultp> size) = 0;
        virtual void SetStencilReference(uint32_t reference) = 0;
        virtual void SetBlendConstant(glm::vec4 color) = 0;
        virtual void SetImmediateData(uint32_t offset, void* data, uint64_t size) = 0;

        virtual void SetBindSet(uint32_t index, BindSet::Ref bindSet, const std::vector<uint32_t>& dynamicOffset) = 0;
        virtual void SetBindSet(uint32_t index, BindSet::Ref bindSet) = 0;

        virtual void SetIndexBuffer(Buffer::Ref buffer, IndexFormat format, uint64_t offset, uint64_t size) = 0;
        virtual void SetIndexBuffer(Buffer::Ref buffer, IndexFormat format, uint64_t offset) = 0;
        virtual void SetIndexBuffer(Buffer::Ref buffer, IndexFormat format) = 0;

        virtual void SetVertexBuffer(uint32_t slot, Buffer::Ref buffer, uint64_t offset, uint64_t size) = 0;
        virtual void SetVertexBuffer(uint32_t slot, Buffer::Ref buffer, uint64_t offset) = 0;
        virtual void SetVertexBuffer(uint32_t slot, Buffer::Ref buffer) = 0;

        virtual void Draw(uint32_t vertexCount, uint32_t vertexOffset) = 0;
        virtual void Draw(uint32_t vertexCount) = 0;
        virtual void DrawInstances(uint32_t vertexCount, uint32_t instanceCount, uint32_t vertexOffset, uint32_t instanceOffset) = 0;
        virtual void DrawInstances(uint32_t vertexCount, uint32_t instanceCount) = 0;
        virtual void DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;
        virtual void DrawIndexed(uint32_t indexCount) = 0;
        virtual void DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t indexOffset, int32_t vertexBase, uint32_t instanceOffset) = 0;
        virtual void DrawInstancesIndexed(uint32_t indexCount, uint32_t instanceCount) = 0;

        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void DrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) = 0;
        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void DrawInstancesIndirect(Buffer::Ref argumentBuffer) = 0;
        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset) = 0;
        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void DrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer) = 0;

        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) = 0;
        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) = 0;
        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) = 0;
        /// Arguments: vertexCount, instanceCount, vertexOffset, instanceOffset
        virtual void MultiDrawInstancesIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) = 0;

        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t byteOffset, uint32_t drawCount) = 0;
        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t drawCount) = 0;
        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint64_t argumentBufferByteOffset, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer, uint64_t drawCountBufferByteOffset) = 0;
        /// Arguments: indexCount, instanceCount, indexOffset, vertexBase, instanceOffset
        virtual void MultiDrawInstancesIndexedIndirect(Buffer::Ref argumentBuffer, uint32_t maxDrawCount, Buffer::Ref drawCountBuffer) = 0;

        virtual void BeginOcclusionQuery(uint32_t index) = 0;
        virtual void EndOcclusionQuery() = 0;

        virtual void ExecuteRenderBundles(const std::vector<RenderBundle::Ref>& renderBundles) = 0;
        virtual void ExecuteRenderBundles(RenderBundle::Ref renderBundle) = 0;
    };
}
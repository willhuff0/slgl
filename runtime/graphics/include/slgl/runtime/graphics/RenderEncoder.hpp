#pragma once

#include "Buffer.hpp"
#include "Texture.hpp"
#include "Sampler.hpp"
#include "BindSet.hpp"
#include "ShaderPipeline.hpp"

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

        virtual void SetPipeline(ShaderPipeline::Render::Ref pipeline) = 0;

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
    };
}
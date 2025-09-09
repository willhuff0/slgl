#pragma once

#include <slgl/runtime/graphics/RenderEncoder.hpp>

#include <webgpu/webgpu.hpp>

#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class RenderEncoder : public Graphics::RenderEncoder {
    public:
        explicit RenderEncoder(wgpu::RenderPassEncoder& renderPass);

        void SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) override;

        void SetBindSet(uint32_t index, Graphics::BindSet::Ref bindSet, const std::vector<uint32_t> &dynamicOffset) override;
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

    private:
        wgpu::RenderPassEncoder& renderPass;
    };
}
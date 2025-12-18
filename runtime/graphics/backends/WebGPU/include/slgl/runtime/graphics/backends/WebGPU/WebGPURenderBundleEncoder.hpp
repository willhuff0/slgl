#pragma once

#include <slgl/runtime/graphics/RenderBundleEncoder.hpp>

#include <webgpu/webgpu.hpp>

#include <cstdint>

namespace SLGL::Graphics::WebGPU {
    class RenderBundleEncoder : public Graphics::RenderBundleEncoder {
    public:
        explicit RenderBundleEncoder(wgpu::RenderBundleEncoder& encoder);

        void InsertDebugMarker(const std::string& label) override;
        void PushDebugScope(const std::string& label) override;
        void PopDebugScope() override;

        void SetPipeline(Graphics::ShaderPipeline::Render::Ref pipeline) override;

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

    private:
        wgpu::RenderBundleEncoder& encoder;
    };
}

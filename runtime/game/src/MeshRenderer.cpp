#include <slgl/runtime/game/MeshRenderer.hpp>

#include <slgl/runtime/game/Provider.hpp>

static const uint64_t ResizableBufferStartSize = 1024;
static const uint64_t ResizableBufferScaleFactor = 2;

ResizableBuffer::ResizableBuffer(const std::string& label, uint8_t usage) : usage(Buffer::Usage::Write | usage) {
    capacity = ResizableBufferStartSize;
    buffer = GetGFX()->CreateBuffer()
        .SetLabel(label)
        .SetSize(capacity)
        .SetUsage(usage)
        .Build();
}

Buffer::Ref ResizableBuffer::GetBuffer() const { return buffer; }

uint64_t ResizableBuffer::Push(void* src, uint64_t pushSize) {
    if (size > capacity - pushSize) {
        capacity *= ResizableBufferScaleFactor;
        auto newBuffer = GetGFX()->CreateBuffer()
                .SetLabel(buffer->GetLabel())
                .SetSize(capacity)
                .SetUsage(usage)
                .Build();

        auto commandEncoder = GetGFX()->CreateCommandEncoder(buffer->GetLabel() + " Resize");
        commandEncoder->CopyBuffer(buffer, newBuffer);
        GetGFX()->SubmitCommands(GetQueue(), commandEncoder->Finish(buffer->GetLabel() + " Resize"));

        buffer = newBuffer;
    }

    uint64_t startOffset = size;

    buffer->Write(GetQueue(), src, size, pushSize);
    size += pushSize;

    return startOffset;
}

void ResizableBuffer::Pop(uint64_t popSize) {
    size -= popSize;
}

MeshRenderer::MeshRenderer()
    : positionBuffer("Mesh Renderer Position Buffer", Buffer::Usage::Vertex),
      normalBuffer("Mesh Renderer Normal Buffer", Buffer::Usage::Vertex),
      indexBuffer("Mesh Renderer Index Buffer", Buffer::Usage::Index),
      indirectArgumentBuffer("Mesh Renderer Indirect Argument Buffer", Buffer::Usage::Indirect),
      instanceBuffer("Mesh Renderer Instance Buffer", Buffer::Usage::Uniform) {
}

uint32_t MeshRenderer::LoadMesh(const std::string& path) {
    return meshes.size() - 1;
}

void MeshRenderer::Draw(RenderEncoder* renderEncoder) {
    renderEncoder->SetPipeline(shaderPipeline);
    renderEncoder->SetBindSet(1, bindSet);

    if (GetGFX()->IsMultiDrawIndirectSupported()) {
        renderEncoder->MultiDrawInstancesIndexedIndirect(indirectArgumentBuffer.GetBuffer(), meshes.size());
    } else {
        for (uint32_t i = 0; i < meshes.size(); i++) {
            renderEncoder->DrawInstancesIndirect(indirectArgumentBuffer.GetBuffer(), i * 5 * sizeof(uint32_t));
        }
    }
}

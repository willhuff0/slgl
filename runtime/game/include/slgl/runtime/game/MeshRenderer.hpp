#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>

#include <cstdint>
#include <unordered_map>

using namespace SLGL::Graphics;

class ResizableBuffer {
public:
    ResizableBuffer(const std::string& label, uint8_t usage);

    [[nodiscard]] Buffer::Ref GetBuffer() const;

    uint64_t Push(void* src, uint64_t size);
    void Pop(uint64_t size);

private:
    uint64_t capacity;
    uint64_t size;

    Buffer::Usage::Flags usage;

    Buffer::Ref buffer;
};

struct MeshInstanceArrayObject {
    uint64_t positionBufferOffset;
    uint64_t normalBufferOffset;
    uint64_t indexBufferOffset;

    uint32_t indexCount;

    std::unordered_map<uint32_t, uint32_t> instanceIndices;
    uint32_t nextInstanceId = 0;
};

struct MeshInstance {

};

class MeshRenderer {
public:
    MeshRenderer();

    uint32_t LoadMesh(const std::string& path);

    void Draw(RenderEncoder* renderEncoder);

private:
    ResizableBuffer positionBuffer;
    ResizableBuffer normalBuffer;
    ResizableBuffer indexBuffer;

    ResizableBuffer indirectArgumentBuffer;

    ResizableBuffer instanceBuffer;

    ShaderPipeline::Render::Ref shaderPipeline;

    BindSet::Layout::Ref bindSetLayout;
    BindSet::Ref bindSet;

    Texture::Ref albedoTexture;

    std::vector<MeshInstanceArrayObject> meshes;
};

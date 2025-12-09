#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/common/Math.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>

using namespace SLGL::Graphics;

struct TerrainUniforms {
    float tiling;
    float ambientIntensity;
};

struct ChunkUniforms {
    glm::mat4 modelMatrix;
};

struct GridPos {
    int x, z;

    bool operator==(const GridPos& other) const {
        return x == other.x && z == other.z;
    }
};
namespace std {
    template<> struct hash<GridPos> {
        std::size_t operator() (const GridPos& chunkPos) const {
            auto h1 = std::hash<int>{}(chunkPos.x);
            auto h2 = std::hash<int>{}(chunkPos.z);
            return h1 ^ (h2 << 1);
        }
    };
}

class Chunk {
public:
    Chunk(GridPos pos, const BindSet::Layout::Ref& bindSetLayout,
          const std::vector<glm::vec3>& positions,
          const std::vector<glm::vec3>& normals,
          const std::vector<uint32_t>& indices
    );

    void Draw(RenderEncoder* renderEncoder);

private:
    Buffer::Ref positionBuffer;
    Buffer::Ref normalBuffer;
    Buffer::Ref indexBuffer;
    BindSet::Ref bindSet;
};

class Terrain {
public:
    explicit Terrain(int seed);

    void Tick(glm::vec3 playerPos);
    void Draw(RenderEncoder* renderEncoder);

private:
    void generateChunk(GridPos pos);

    int seed;

    BindSet::Layout::Ref chunkBindSetLayout;
    BindSet::Ref bindSet;
    ShaderPipeline::Layout::Ref shaderPipelineLayout;
    ShaderPipeline::Render::Ref shaderPipeline;

    std::unordered_map<GridPos, Chunk> chunks;
    std::unordered_set<GridPos> visibleChunks;
};
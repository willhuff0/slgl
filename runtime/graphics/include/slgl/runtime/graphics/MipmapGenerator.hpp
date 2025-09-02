#pragma once

#include "GFX.hpp"

#include <unordered_map>

namespace SLGL::Graphics {
    class MipmapGenerator {
    public:
        MipmapGenerator(GFX* gfx, const Data::Shader& shader);

        void GenerateMipmaps(GFX* gfx, Queue* queue, const Texture::Ref& texture, int mipLevels);

    private:
        ShaderModule::Ref shaderModule;
        BindSet::Layout::Ref bindSetLayout;
        ShaderPipeline::Layout::Ref shaderPipelineLayout;
        Sampler::Ref sampler;
        std::unordered_map<Texture::Format, ShaderPipeline::Render::Ref> shaderRenderPipelines;
    };
}

#pragma once

#include "Backend.hpp"

#include <unordered_map>

namespace SLGL::Graphics {
    class MipmapGenerator {
    public:
        MipmapGenerator(Backend* gfx, const Data::Shader& shader);

        void GenerateMipmaps(Backend* gfx, Queue* queue, const Texture::Ref& texture, int mipLevels);

    private:
        ShaderModule::Ref shaderModule;
        BindSet::Layout::Ref bindSetLayout;
        ShaderPipeline::Layout::Ref shaderPipelineLayout;
        Sampler::Ref sampler;
        std::unordered_map<Texture::Format, ShaderPipeline::Render::Ref> shaderRenderPipelines;
    };
}

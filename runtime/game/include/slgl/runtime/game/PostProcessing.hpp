#pragma once

#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/common/Math.hpp>

using namespace SLGL::Graphics;

enum class ToneMappingMode : uint32_t {
    None = 0,
    Reinhard = 1,
    ACES = 2,
};

struct PostProcessingUniforms {
    float bloomThreshold;
    float acesExposure;
    ToneMappingMode toneMappingMode;
};

class PostProcessing {
public:
    explicit PostProcessing();

    void ResizeRenderTexture(glm::ivec2 newSize);
    void SetUniforms(PostProcessingUniforms* uniforms);

    Texture::View::Ref GetPostProcessingInputTextureView();

    void PostProcessingRenderPass(const CommandEncoder::Ref& commandEncoder, const Texture::View::Ref& surfaceTextureView);

private:
    BindSet::Layout::Ref bindSetLayout;
    Buffer::Ref uniformBuffer;

    ShaderPipeline::Render::Ref splitBrightnessPipeline;
    ShaderPipeline::Render::Ref blurHorizontalPipeline;
    ShaderPipeline::Render::Ref blurVerticalPipeline;
    ShaderPipeline::Render::Ref colorCorrectionPipeline;

    BindSet::Ref inputBindSet;
    BindSet::Ref bindSet1;
    BindSet::Ref bindSet2;

    Texture::View::Ref inputRenderTextureView;
    Texture::View::Ref renderTextureView1;
    Texture::View::Ref renderTextureView2;
};
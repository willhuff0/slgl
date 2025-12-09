#include <slgl/runtime/game/PostProcessing.hpp>

#include <slgl/runtime/game/Provider.hpp>

#include <string>

const int numBlurIterations = 5;
const float bloomThreshold = 1.0f;
const float acesExposure = 1.2f;
const ToneMappingMode toneMappingMode = ToneMappingMode::ACES;

static const std::string fullscreenVertexShaderCode = R"(
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoords: vec2f,
};

@vertex
fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
    let positions = array(
        // 1st triangle
        vec2f(0.0,  0.0),  // center
        vec2f(1.0,  0.0),  // right, center
        vec2f(0.0,  1.0),  // center, top

        // 2nd triangle
        vec2f(0.0,  1.0),  // center, top
        vec2f(1.0,  0.0),  // right, center
        vec2f(1.0,  1.0),  // right, top
    );

    var out: VertexOutput;
    let xy = positions[vertexIndex];
    out.position = vec4f(xy * 2.0 - 1.0, 0.0, 1.0);
    out.texCoords = vec2f(xy.x, 1.0 - xy.y);
    return out;
}
)";

static const std::string postProcessingUniformsCode = R"(
struct PostProcessingUniforms {
    bloomThreshold: f32,
    exposure: f32,
    toneMappingMode: u32,
};
@group(0) @binding(0) var<uniform> u_postProcessing: PostProcessingUniforms;
)";

static const std::string splitBrightnessShaderCode = postProcessingUniformsCode + R"(
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoords: vec2f,
};

@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var u_texture: texture_2d<f32>;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let color = textureSample(u_texture, u_sampler, in.texCoords).rgb;
    let luminance = dot(color, vec3f(0.2126, 0.7152, 0.0722));
    return select(vec4f(0.0), vec4f(color, 1.0), luminance > u_postProcessing.bloomThreshold);
}
)";

static const std::string blurShaderCode = postProcessingUniformsCode + R"(
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoords: vec2f,
};

@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var u_texture: texture_2d<f32>;

override c_horizontal: bool;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let weight = array(0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

    let texOffset = 1.0 / vec2f(textureDimensions(u_texture));
    var result = textureSample(u_texture, u_sampler, in.texCoords).rgb * weight[0];
    if (c_horizontal) {
        for (var i = 1; i < 5; i++) {
            result += textureSample(u_texture, u_sampler, in.texCoords + vec2f(texOffset.x * f32(i), 0.0)).rgb * weight[i];
            result += textureSample(u_texture, u_sampler, in.texCoords - vec2f(texOffset.x * f32(i), 0.0)).rgb * weight[i];
        }
    } else {
        for (var i = 1; i < 5; i++) {
            result += textureSample(u_texture, u_sampler, in.texCoords + vec2f(0.0, texOffset.y * f32(i))).rgb * weight[i];
            result += textureSample(u_texture, u_sampler, in.texCoords - vec2f(0.0, texOffset.y * f32(i))).rgb * weight[i];
        }
    }

    return vec4f(result, 1.0);
}
)";

static const std::string colorCorrectionShaderCode = postProcessingUniformsCode + R"(
struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) texCoords: vec2f,
};

@group(0) @binding(1) var u_sampler: sampler;
@group(0) @binding(2) var u_baseTexture: texture_2d<f32>;
@group(1) @binding(2) var u_bloomTexture: texture_2d<f32>;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let baseColor = textureSample(u_baseTexture, u_sampler, in.texCoords).rgb;
    let bloomColor = textureSample(u_bloomTexture, u_sampler, in.texCoords).rgb;

    var color = baseColor + bloomColor;

    if (u_postProcessing.toneMappingMode == 1) { color = color / (color + vec3f(1.0)); }
    else if (u_postProcessing.toneMappingMode == 2) { color = 1.0 - exp(-color * u_postProcessing.exposure); }

    color = pow(color, vec3f(1.0 / 2.2));

    return vec4f(color, 1.0);
}
)";

PostProcessing::PostProcessing() {
    bindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("Post Processing")
        .AddEntry(0, {
            .type = BindSet::Layout::Entry::Type::Buffer,
            .buffer = {
                .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
                .minSize = sizeof(PostProcessingUniforms),
            },
        })
        .AddEntry(1, {
            .type = BindSet::Layout::Entry::Type::Sampler,
            .sampler = { },
        })
        .AddEntry(2, {
            .type = BindSet::Layout::Entry::Type::Texture,
            .texture = { },
        })
        .Build();

    uniformBuffer = GetGFX()->CreateBuffer()
        .SetLabel("Post Processing Uniform Buffer")
        .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
        .SetSize(sizeof(PostProcessingUniforms))
        .Build();

    PostProcessingUniforms uniforms {
        .bloomThreshold = bloomThreshold,
        .acesExposure = acesExposure,
        .toneMappingMode = toneMappingMode,
    };
    uniformBuffer->Write(GetQueue(), &uniforms);

    auto pipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("Post Processing")
        .AddBindSetLayout(bindSetLayout)
        .Build();

    auto fullscreenVertexShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Post Processing - Vertex")
        .SetSource(GetGFX()->CreateShaderSource({
            .language = Data::Shader::Language::WGSL,
            .source = fullscreenVertexShaderCode,
        }))
        .Build();

    Data::Shader splitBrightnessShaderData {
        .language = Data::Shader::Language::WGSL,
        .source = splitBrightnessShaderCode,
    };
    auto splitBrightnessShaderSource = GetGFX()->CreateShaderSource(splitBrightnessShaderData);
    auto splitBrightnessShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Post Processing - Split Brightness")
        .SetSource(splitBrightnessShaderSource)
        .Build();

    splitBrightnessPipeline = GetGFX()->CreateShaderRenderPipeline(pipelineLayout)
        .SetLabel("Post Processing - Split Brightness")
        .SetVertex({
            .module = fullscreenVertexShaderModule,
            .entry = "vs_main",
        })
        .SetFragment({
            .module = splitBrightnessShaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetRenderTextureSurfaceFormat(),
                },
            },
        })
        .Build();

    Data::Shader blurShaderData {
        .language = Data::Shader::Language::WGSL,
        .source = blurShaderCode,
    };
    auto blurShaderSource = GetGFX()->CreateShaderSource(blurShaderData);
    auto blurShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Post Processing - Blur")
        .SetSource(blurShaderSource)
        .Build();

    blurHorizontalPipeline = GetGFX()->CreateShaderRenderPipeline(pipelineLayout)
        .SetLabel("Post Processing - Blur Horizontal")
        .SetVertex({
            .module = fullscreenVertexShaderModule,
            .entry = "vs_main",
        })
        .SetFragment({
            .module = blurShaderModule,
            .entry = "fs_main",
            .constants = {
                { "c_horizontal", 1 },
            },
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetRenderTextureSurfaceFormat(),
                },
            },
        })
        .Build();

    blurVerticalPipeline = GetGFX()->CreateShaderRenderPipeline(pipelineLayout)
        .SetLabel("Post Processing - Blur Vertical")
        .SetVertex({
            .module = fullscreenVertexShaderModule,
            .entry = "vs_main",
        })
        .SetFragment({
            .module = blurShaderModule,
            .entry = "fs_main",
            .constants = {
                { "c_horizontal", 0 },
            },
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetRenderTextureSurfaceFormat(),
                },
            },
        })
        .Build();

    Data::Shader colorCorrectionShaderData {
        .language = Data::Shader::Language::WGSL,
        .source = colorCorrectionShaderCode,
    };
    auto colorCorrectionShaderSource = GetGFX()->CreateShaderSource(colorCorrectionShaderData);
    auto colorCorrectionShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Post Processing - Color Correction")
        .SetSource(colorCorrectionShaderSource)
        .Build();

    colorCorrectionPipeline = GetGFX()->CreateShaderRenderPipeline(GetGFX()->CreateShaderPipelineLayout()
            .SetLabel("Post Processing - Combine / Color Correction")
            .AddBindSetLayout(bindSetLayout)
            .AddBindSetLayout(bindSetLayout)
            .Build())
        .SetLabel("Post Processing - Combine / Color Correction")
        .SetVertex({
            .module = fullscreenVertexShaderModule,
            .entry = "vs_main",
        })
        .SetFragment({
            .module = colorCorrectionShaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetDisplaySurfaceFormat(),
                },
            },
        })
        .Build();
}

void PostProcessing::ResizeRenderTexture(glm::ivec2 newSize) {
    auto renderTextureBuilder = GetGFX()->CreateTexture()
        .SetUsage(Texture::Usage::Texture | Texture::Usage::RenderTarget)
        .SetFormat(GetRenderTextureFormat())
        .SetSize(glm::ivec3(newSize, 1));

    inputRenderTextureView = renderTextureBuilder
        .SetLabel("Pre Post Processing Render Texture")
        .Build()->CreateView().Build();
    renderTextureView1 = renderTextureBuilder
        .SetLabel("Post Processing Intermediate Render Texture 1")
        .Build()->CreateView().Build();
    renderTextureView2 = renderTextureBuilder
        .SetLabel("Post Processing Intermediate Render Texture 2")
        .Build()->CreateView().Build();

    auto bindSetBuilder = bindSetLayout->Instantiate()
        .SetEntry(0, { BindSet::Entry::Buffer {
            .buffer = uniformBuffer,
            .size = uniformBuffer->GetSize(),
        }})
        .SetEntry(1, { BindSet::Entry::Sampler {
            .sampler = GetGFX()->CreateSampler()
                .SetLabel("Post Processing")
                .Build(),
        }});

    inputBindSet = bindSetBuilder
        .SetLabel("Post Processing - Input")
        .SetEntry(2, { BindSet::Entry::TextureView {
            .textureView = inputRenderTextureView,
        }})
        .Build();
    bindSet1 = bindSetBuilder
        .SetLabel("Post Processing - Intermediate 1")
        .SetEntry(2, { BindSet::Entry::TextureView {
            .textureView = renderTextureView1,
        }})
        .Build();
    bindSet2 = bindSetBuilder
        .SetLabel("Post Processing - Intermediate 2")
        .SetEntry(2, { BindSet::Entry::TextureView {
            .textureView = renderTextureView2,
        }})
        .Build();
}

void PostProcessing::SetUniforms(PostProcessingUniforms* uniforms) {
    uniformBuffer->Write(GetQueue(), &uniforms);
}

Texture::View::Ref PostProcessing::GetPrePostProcessingRenderTextureView() { return inputRenderTextureView; }

void PostProcessing::PostProcessingRenderPass(const CommandEncoder::Ref& commandEncoder, const Texture::View::Ref& surfaceTextureView) {
    commandEncoder->EncodeRenderPass({
            .label = "Post Processing - Split Brightness (Input -> 1)",
            .colorAttachments = {
                CommandEncoder::RenderPass::ColorAttachment {
                    .textureView = renderTextureView1,
                },
            },
        }, [&](RenderEncoder* renderEncoder){
        renderEncoder->SetPipeline(splitBrightnessPipeline);
        renderEncoder->SetBindSet(0, inputBindSet);
        renderEncoder->Draw(6);
    });

    bool horizontal = true;
    std::string pingPongLabels[2] = { "Horizontal (1 -> 2)", "Vertical (2 -> 1)" };
    Texture::View::Ref pingPongTargets[2] = { renderTextureView1, renderTextureView2 };
    BindSet::Ref pingPongBindSets[2] = { bindSet2, bindSet1 };
    ShaderPipeline::Render::Ref pingPongPipelines[2] = { blurVerticalPipeline, blurHorizontalPipeline };

    for (int i = 0; i < numBlurIterations * 2; i++) {
        commandEncoder->EncodeRenderPass({
                .label = "Post Processing - Blur Pass " + std::to_string(i) + ", " + pingPongLabels[horizontal],
                .colorAttachments = {
                    CommandEncoder::RenderPass::ColorAttachment {
                        .textureView = pingPongTargets[horizontal],
                    },
                },
            }, [&](RenderEncoder* renderEncoder){
            renderEncoder->SetPipeline(pingPongPipelines[horizontal]);
            renderEncoder->SetBindSet(0, pingPongBindSets[horizontal]);
            renderEncoder->Draw(6);
        });
        horizontal = !horizontal;
    }

    commandEncoder->EncodeRenderPass({
            .label = "Post Processing - Color Correction (1 -> Surface)",
            .colorAttachments = {
                CommandEncoder::RenderPass::ColorAttachment {
                    .textureView = surfaceTextureView,
                },
            },
        }, [&](RenderEncoder* renderEncoder){
        renderEncoder->SetPipeline(colorCorrectionPipeline);
        renderEncoder->SetBindSet(0, inputBindSet);
        renderEncoder->SetBindSet(1, bindSet1);
        renderEncoder->Draw(6);
    });
}
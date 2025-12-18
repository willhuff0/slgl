#include <slgl/runtime/game/Skybox.hpp>

#include <slgl/runtime/game/Provider.hpp>

#include <string>

static const int IRRADIANCE_MAP_SIZE = 64;
static const int RADIANCE_MAP_SIZE = 512;
static const int BRDF_LUT_SIZE = 512;
static const int RADIANCE_MIPMAP_COUNT = (int)std::floor(std::log2(512)) + 1;

static const float cubeVertexPositions[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

static const std::string cubeMapConverterVertexShaderCode = R"(
struct CubeMapCaptureUniforms {
    viewProjMatrix: mat4x4f,
};
@group(0) @binding(0) var<uniform> u_capture: CubeMapCaptureUniforms;

struct VertexInput {
    @location(0) position: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) localPosition: vec3f,
};

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = u_capture.viewProjMatrix * vec4f(in.position, 1.0);
    out.localPosition = in.position;
    return out;
}
)";

static const std::string equirectangularToCubeMapFragmentShaderCode = R"(
@group(1) @binding(0) var u_equirectangularMap: texture_2d<f32>;
@group(1) @binding(1) var u_sampler: sampler;

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) localPosition: vec3f,
};

const PI = 3.14159265359;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let position = normalize(in.localPosition);

    var uv = vec2f(atan2(position.x, position.z), asin(position.y));
    uv *= vec2f(1.0 / (2.0 * PI), 1.0 / PI);
    uv += 0.5;

    var color = textureSample(u_equirectangularMap, u_sampler, uv).rgb;

    // Tone mapping - Reinhard
//    color = color / (color + vec3f(1.0));

    // Tone mapping - ACES
//    color = 1.0 - exp(-color * 1.0);

    color = min(color, vec3f(100.0));

    return vec4f(color, 1.0);
}
)";

static const std::string irradianceFragmentShaderCode = R"(
@group(1) @binding(0) var u_cubeMap: texture_cube<f32>;
@group(1) @binding(1) var u_sampler: sampler;

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) localPosition: vec3f,
};

const PI = 3.14159265359;

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let normal = normalize(in.localPosition);

    var irradiance = vec3f(0.0);

    var up = vec3f(0.0, 1.0, 0.0);
    let right = normalize(cross(up, normal));
    up = normalize(cross(normal, right));

    let sampleDelta = 0.025;
    var nrSamples = 0.0;
    for (var phi = 0.0; phi < 2.0 * PI; phi += sampleDelta) {
        for (var theta = 0.0; theta < 0.5 * PI; theta += sampleDelta) {
            let tangentSample = vec3f(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            let sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            let sample = textureSample(u_cubeMap, u_sampler, sampleVec * vec3f(-1, 1, 1)).rgb * cos(theta) * sin(theta);
            irradiance += sample;

            nrSamples += 1.0;
        }
    }
    irradiance = PI * irradiance * (1.0 / nrSamples);

    return vec4f(irradiance, 1.0);
}
)";

static const std::string radianceFragmentShaderCode = R"(
@group(1) @binding(0) var u_cubeMap: texture_cube<f32>;
@group(1) @binding(1) var u_sampler: sampler;

struct RadianceUniforms {
    roughness: f32,
};
@group(1) @binding(2) var<uniform> u_radiance: RadianceUniforms;

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) localPosition: vec3f,
};

const PI = 3.14159265359;

fn RadicalInverseVdC(bits: u32) -> f32 {
    var b: u32 = bits;

    b = (b << 16u) | (b >> 16u);
    b = ((b & 0x55555555u) << 1u)  | ((b & 0xAAAAAAAAu) >> 1u);
    b = ((b & 0x33333333u) << 2u)  | ((b & 0xCCCCCCCCu) >> 2u);
    b = ((b & 0x0F0F0F0Fu) << 4u)  | ((b & 0xF0F0F0F0u) >> 4u);
    b = ((b & 0x00FF00FFu) << 8u)  | ((b & 0xFF00FF00u) >> 8u);

    return f32(b) * 2.3283064365386963e-10f;
}
fn Hammersley(i: u32, N: u32) -> vec2f {
    return vec2f(f32(i)/f32(N), RadicalInverseVdC(i));
}

fn ImportanceSampleGGX(Xi: vec2f, N: vec3f, roughness: f32) -> vec3f {
    let a = roughness * roughness;

    let phi = 2.0 * PI * Xi.x;
    let cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    let sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    let H = vec3f(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    let up = select(vec3f(1.0, 0.0, 0.0), vec3f(0.0, 0.0, 1.0), abs(N.z) < 0.999);
    let tangent = normalize(cross(up, N));
    let bitangent = cross(N, tangent);

    let sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    let N = normalize(in.localPosition);
    let R = N;
    let V = R;

    const SAMPLE_COUNT = 1024u;
    var totalWeight = 0.0;
    var prefilteredColor = vec3f(0.0);
    for (var i = 0u; i < SAMPLE_COUNT; i++) {
        let Xi = Hammersley(i, SAMPLE_COUNT);
        let H = ImportanceSampleGGX(Xi, N, u_radiance.roughness);
        let L = normalize(2.0 * dot(V, H) * H - V);

        let NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0) {
            prefilteredColor += textureSampleLevel(u_cubeMap, u_sampler, L * vec3f(-1, 1, 1), 1.0).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;

    return vec4f(prefilteredColor, 1.0);
}
)";

CubeMapConverter::CubeMapConverter() {
    perFaceBindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("CubeMapConverter Per Face")
        .AddEntry(0, {
            .type = BindSet::Layout::Entry::Type::Buffer,
            .buffer = {
                .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
                .minSize = sizeof(CubeMapCaptureUniforms),
            },
        })
        .Build();

    equirectangularToCubeMapPerMapBindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("CubeMapConverter EquirectangularToCubeMap Per Map")
        .AddEntry(0, { .type = BindSet::Layout::Entry::Type::Texture, .texture = { } })
        .AddEntry(1, { .type = BindSet::Layout::Entry::Type::Sampler, .sampler = { } })
        .Build();

    convolutionPerMapBindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("CubeMapConverter Convolution Per Map")
        .AddEntry(0, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {
            .viewDimension = Texture::View::Dimension::Cube,
        }})
        .AddEntry(1, { .type = BindSet::Layout::Entry::Type::Sampler, .sampler = { } })
        .AddEntry(2, { .type = BindSet::Layout::Entry::Type::Buffer, .buffer = {
            .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
            .minSize = 4,
        }})
        .Build();

    auto equirectangularToCubeMapShaderPipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("CubeMapConverter EquirectangularToCubeMap")
        .AddBindSetLayout(perFaceBindSetLayout)
        .AddBindSetLayout(equirectangularToCubeMapPerMapBindSetLayout)
        .Build();

    auto convolutionShaderPipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("CubeMapConverter Convolution")
        .AddBindSetLayout(perFaceBindSetLayout)
        .AddBindSetLayout(convolutionPerMapBindSetLayout)
        .Build();

    Data::Shader vertexShaderData {
        .language = Data::Shader::Language::WGSL,
        .source = cubeMapConverterVertexShaderCode,
    };
    auto vertexShaderSource = GetGFX()->CreateShaderSource(vertexShaderData);
    auto vertexShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("CubeMapConverter Vertex")
        .SetSource(vertexShaderSource)
        .Build();

    Data::Shader equirectangularToCubeMapShaderData {
        .language = Data::Shader::Language::WGSL,
        .source = equirectangularToCubeMapFragmentShaderCode,
    };
    auto equirectangularToCubeMapShaderSource = GetGFX()->CreateShaderSource(equirectangularToCubeMapShaderData);
    auto equirectangularToCubeMapShaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("CubeMapConverter EquirectangularToCubeMap Fragment")
        .SetSource(equirectangularToCubeMapShaderSource)
        .Build();

    Data::Shader irradianceShaderData {
            .language = Data::Shader::Language::WGSL,
            .source = irradianceFragmentShaderCode,
    };
    auto irradianceShaderSource = GetGFX()->CreateShaderSource(irradianceShaderData);
    auto irradianceShaderModule = GetGFX()->CreateShaderModule()
            .SetLabel("CubeMapConverter Irradiance Fragment")
            .SetSource(irradianceShaderSource)
            .Build();

    Data::Shader radianceShaderData {
            .language = Data::Shader::Language::WGSL,
            .source = radianceFragmentShaderCode,
    };
    auto radianceShaderSource = GetGFX()->CreateShaderSource(radianceShaderData);
    auto radianceShaderModule = GetGFX()->CreateShaderModule()
            .SetLabel("CubeMapConverter Radiance Fragment")
            .SetSource(radianceShaderSource)
            .Build();

    equirectangularToCubeMapShaderPipeline = GetGFX()->CreateShaderRenderPipeline(equirectangularToCubeMapShaderPipelineLayout)
        .SetLabel("CubeMapConverter EquirectangularToCubeMap")
        .SetVertex({
            .module = vertexShaderModule,
            .entry = "vs_main",
            .buffers = {
                ShaderPipeline::Render::Vertex::Buffer {
                    .stride = sizeof(glm::vec3),
                    .attributes = {
                        ShaderPipeline::Render::Vertex::Buffer::Attribute {
                            .format = ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3,
                            .location = 0,
                        },
                    },
                },
            },
        })
        .SetFragment({
            .module = equirectangularToCubeMapShaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetGFX()->GetSurfaceTextureFormat(Texture::Format::RGBA16Float, Texture::ColorSpace::Linear),
                },
            },
        })
        .Build();

    irradianceShaderPipeline = GetGFX()->CreateShaderRenderPipeline(convolutionShaderPipelineLayout)
        .SetLabel("CubeMapConverter Irradiance")
        .SetVertex({
            .module = vertexShaderModule,
            .entry = "vs_main",
            .buffers = {
                ShaderPipeline::Render::Vertex::Buffer {
                    .stride = sizeof(glm::vec3),
                    .attributes = {
                        ShaderPipeline::Render::Vertex::Buffer::Attribute {
                            .format = ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3,
                            .location = 0,
                        },
                    },
                },
            },
        })
        .SetFragment({
            .module = irradianceShaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetGFX()->GetSurfaceTextureFormat(Texture::Format::RGBA16Float, Texture::ColorSpace::Linear),
                },
            },
        })
        .Build();

    radianceShaderPipeline = GetGFX()->CreateShaderRenderPipeline(convolutionShaderPipelineLayout)
        .SetLabel("CubeMapConverter Radiance")
        .SetVertex({
            .module = vertexShaderModule,
            .entry = "vs_main",
            .buffers = {
                ShaderPipeline::Render::Vertex::Buffer {
                    .stride = sizeof(glm::vec3),
                    .attributes = {
                        ShaderPipeline::Render::Vertex::Buffer::Attribute {
                            .format = ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3,
                            .location = 0,
                        },
                    },
                },
            },
        })
        .SetFragment({
            .module = radianceShaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetGFX()->GetSurfaceTextureFormat(Texture::Format::RGBA16Float, Texture::ColorSpace::Linear),
                },
            },
        })
        .Build();

    positionBuffer = GetGFX()->CreateBuffer()
        .SetLabel("CubeMapConverter Position Buffer")
        .SetUsage(Buffer::Usage::Vertex | Buffer::Usage::Write)
        .SetSize(sizeof(cubeVertexPositions))
        .Build();
    positionBuffer->Write(GetQueue(), (void*)cubeVertexPositions);

    sampler = GetGFX()->CreateSampler()
        .SetLabel("CubeMapConverter")
        .SetAddressModeU(Sampler::AddressMode::Clamp)
        .SetAddressModeV(Sampler::AddressMode::Clamp)
        .SetAddressModeW(Sampler::AddressMode::Clamp)
        .SetMinFilter(Sampler::FilterMode::Linear)
        .SetMagFilter(Sampler::FilterMode::Linear)
        .Build();

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] = {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f))
    };

    faceBindSets.reserve(6);
    for (int i = 0; i < 6; i++) {
        auto uniformBuffer = GetGFX()->CreateBuffer()
            .SetLabel("CubeMapConverter Uniform Buffer (" + std::to_string(i) + ")")
            .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
            .SetSize(sizeof(CubeMapCaptureUniforms))
            .Build();

        CubeMapCaptureUniforms uniforms {
            .viewProjMatrix = captureProjection * captureViews[i],
        };
        uniformBuffer->Write(GetQueue(), &uniforms);

        faceBindSets.emplace_back(perFaceBindSetLayout->Instantiate()
            .SetLabel("CubeMapConverter Face (" + std::to_string(i) + ")")
            .SetEntry(0, { BindSet::Entry::Buffer {
                .buffer = uniformBuffer,
                .size = uniformBuffer->GetSize(),
            }})
            .Build());
    }

    convolutionUniformBuffer = GetGFX()->CreateBuffer()
        .SetLabel("CubeMapConverter Convolution Uniform Buffer")
        .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
        .SetSize(4)
        .Build();
    ConvolutionUniforms uniforms {
        .roughness = 1.0,
    };
    convolutionUniformBuffer->Write(GetQueue(), &uniforms);
}

Texture::Ref CubeMapConverter::ConvertEquirectangularToCubeMap(const Texture::Ref& equirectangularMap, const std::string& label) {
    int cubeMapFaceSize = equirectangularMap->GetSize().y;
    auto cubeMapTexture = GetGFX()->CreateTexture()
        .SetLabel(label)
        .SetUsage(Texture::Usage::Texture | Texture::Usage::RenderTarget)
        .SetFormat(Texture::Format::RGBA16Float)
        .SetColorSpace(Texture::ColorSpace::Linear)
        .SetDimension(Texture::Dimension::_2D)
        .SetSize({ cubeMapFaceSize, cubeMapFaceSize, 6 })
        .Build();

    auto perMapBindSet = equirectangularToCubeMapPerMapBindSetLayout->Instantiate()
        .SetLabel("CubeMapConverter ConvertEquirectangularToCubeMap Per Map")
        .SetEntry(0, { BindSet::Entry::TextureView {
            .textureView = equirectangularMap->CreateView().Build(),
        }})
        .SetEntry(1, { BindSet::Entry::Sampler {
            .sampler = sampler,
        }})
        .Build();

    capture(perMapBindSet, cubeMapTexture, equirectangularToCubeMapShaderPipeline);

    return cubeMapTexture;
}

Texture::Ref CubeMapConverter::GenerateIrradianceMap(const Texture::Ref& cubeMap, const std::string& label) {
    auto irradianceMap = GetGFX()->CreateTexture()
        .SetLabel(label)
        .SetUsage(Texture::Usage::Texture | Texture::Usage::RenderTarget)
        .SetFormat(cubeMap->GetFormat())
        .SetColorSpace(Texture::ColorSpace::Linear)
        .SetDimension(Texture::Dimension::_2D)
        .SetSize({ IRRADIANCE_MAP_SIZE, IRRADIANCE_MAP_SIZE, 6 })
        .Build();

    auto perMapBindSet = convolutionPerMapBindSetLayout->Instantiate()
        .SetLabel("CubeMapConverter Irradiance Per Map")
        .SetEntry(0, { BindSet::Entry::TextureView {
            .textureView = cubeMap->CreateView()
                .SetDimension(Texture::View::Dimension::Cube)
                .SetArrayLayerCount(6)
                .Build(),
        }})
        .SetEntry(1, { BindSet::Entry::Sampler {
            .sampler = sampler,
        }})
        .SetEntry(2, { BindSet::Entry::Buffer {
            .buffer = convolutionUniformBuffer,
            .size = 4,
        }})
        .Build();

    capture(perMapBindSet, irradianceMap, irradianceShaderPipeline);

    return irradianceMap;
}

Texture::Ref CubeMapConverter::GenerateRadianceMap(const Texture::Ref& cubeMap, const std::string& label) {
    auto radianceMap = GetGFX()->CreateTexture()
        .SetLabel(label)
        .SetUsage(Texture::Usage::Texture | Texture::Usage::RenderTarget)
        .SetFormat(cubeMap->GetFormat())
        .SetColorSpace(Texture::ColorSpace::Linear)
        .SetDimension(Texture::Dimension::_2D)
        .SetSize({RADIANCE_MAP_SIZE, RADIANCE_MAP_SIZE, 6})
        .SetMipLevelCount(RADIANCE_MIPMAP_COUNT)
        .Build();

    auto perMapBindSet = convolutionPerMapBindSetLayout->Instantiate()
        .SetLabel("CubeMapConverter Radiance Per Map")
        .SetEntry(0, { BindSet::Entry::TextureView {
            .textureView = cubeMap->CreateView()
                .SetDimension(Texture::View::Dimension::Cube)
                .SetArrayLayerCount(6)
                .Build(),
        }})
        .SetEntry(1, { BindSet::Entry::Sampler {
            .sampler = sampler,
        }})
        .SetEntry(2, { BindSet::Entry::Buffer {
            .buffer = convolutionUniformBuffer,
            .size = 4,
        }})
        .Build();

    for (int i = 0; i < radianceMap->GetMipLevelCount(); i++) {
        ConvolutionUniforms uniforms {
            .roughness = (float)i / (float)(radianceMap->GetMipLevelCount() - 1),
        };
        convolutionUniformBuffer->Write(GetQueue(), &uniforms);
        capture(perMapBindSet, radianceMap, radianceShaderPipeline, i);
    }

    return radianceMap;
}

void CubeMapConverter::capture(const BindSet::Ref& perMapBindSet, const Texture::Ref& target, ShaderPipeline::Render::Ref shaderPipeline, int mipMap) {
    auto commandEncoder = GetGFX()->CreateCommandEncoder("CubeMapConverter");
    for (int i = 0; i < 6; i++) {
        auto cubeMapTextureView = target->CreateView()
            .SetDimension(Texture::View::Dimension::_2D)
            .SetArrayLayerCount(1)
            .SetMipLevelCount(1)
            .SetBaseArrayLayer(i)
            .SetBaseMipLevel(mipMap)
            .Build();

        commandEncoder->EncodeRenderPass({
            .label = "CubeMapConverter",
            .colorAttachments = {
                RenderPass::ColorAttachment {
                    .textureView = cubeMapTextureView,
                    .clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0),
                },
            },
        }, [&](RenderEncoder* renderEncoder){
            renderEncoder->SetPipeline(shaderPipeline);
            renderEncoder->SetBindSet(0, faceBindSets.at(i));
            renderEncoder->SetBindSet(1, perMapBindSet);
            renderEncoder->SetVertexBuffer(0, positionBuffer);
            renderEncoder->Draw(36);
        });
    }
    auto commandBuffer = commandEncoder->Finish("CubeMapConverter");
    GetGFX()->SubmitCommands(GetQueue(), commandBuffer);
}

static const std::string envBrdfLutGeneratorShaderCode = R"(
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

const PI = 3.14159265359;

fn RadicalInverseVdC(bits: u32) -> f32 {
    var b: u32 = bits;

    b = (b << 16u) | (b >> 16u);
    b = ((b & 0x55555555u) << 1u)  | ((b & 0xAAAAAAAAu) >> 1u);
    b = ((b & 0x33333333u) << 2u)  | ((b & 0xCCCCCCCCu) >> 2u);
    b = ((b & 0x0F0F0F0Fu) << 4u)  | ((b & 0xF0F0F0F0u) >> 4u);
    b = ((b & 0x00FF00FFu) << 8u)  | ((b & 0xFF00FF00u) >> 8u);

    return f32(b) * 2.3283064365386963e-10f;
}
fn Hammersley(i: u32, N: u32) -> vec2f {
    return vec2f(f32(i)/f32(N), RadicalInverseVdC(i));
}

fn ImportanceSampleGGX(Xi: vec2f, N: vec3f, roughness: f32) -> vec3f {
    let a = roughness * roughness;

    let phi = 2.0 * PI * Xi.x;
    let cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    let sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    let H = vec3f(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    let up = select(vec3f(1.0, 0.0, 0.0), vec3f(0.0, 0.0, 1.0), abs(N.z) < 0.999);
    let tangent = normalize(cross(up, N));
    let bitangent = cross(N, tangent);

    let sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

fn GeometrySchlickGGX(NdotV: f32, roughness: f32) -> f32 {
//    let r = (roughness + 1.0);
//    let k = (r * r) / 8.0;
    let r = roughness;
    let k = (r * r) / 2.0;

    let num = NdotV;
    let denom = NdotV * (1.0 - k) + k;

    return num / denom;
}
fn GeometrySmith(N: vec3f, V: vec3f, L: vec3f, roughness: f32) -> f32 {
    let NdotV = max(dot(N, V), 0.0);
    let NdotL = max(dot(N, L), 0.0);
    let ggx2 = GeometrySchlickGGX(NdotV, roughness);
    let ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

fn IntegrateBRDF(NdotV: f32, roughness: f32) -> vec2f {
    let V = vec3f(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);

    var A = 0.0;
    var B = 0.0;

    let N = vec3f(0.0, 0.0, 1.0);

    const sampleCount = 1024u;
    for (var i = 0u; i < sampleCount; i++) {
        let Xi = Hammersley(i, sampleCount);
        let H = ImportanceSampleGGX(Xi, N, roughness);
        let L = normalize(2.0 * dot(V, H) * H - V);

        let NdotL = max(L.z, 0.0);
        let NdotH = max(H.z, 0.0);
        let VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0) {
            let G = GeometrySmith(N, V, L, roughness);
            let G_Vis = (G * VdotH) / (NdotH * NdotV);
            let Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    A /= f32(sampleCount);
    B /= f32(sampleCount);
    return vec2f(A, B);
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec2f {
    return IntegrateBRDF(in.texCoords.x, in.texCoords.y);
}
)";

EnvBrdfLutGenerator::EnvBrdfLutGenerator() {
    auto pipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("Environment BRDF LUT Generator")
        .Build();

    Data::Shader shaderData {
            .language = Data::Shader::Language::WGSL,
            .source = envBrdfLutGeneratorShaderCode,
    };
    auto shaderSource = GetGFX()->CreateShaderSource(shaderData);
    auto shaderModule = GetGFX()->CreateShaderModule()
            .SetLabel("Environment BRDF LUT Generator")
            .SetSource(shaderSource)
            .Build();

    shaderPipeline = GetGFX()->CreateShaderRenderPipeline(pipelineLayout)
        .SetLabel("Environment BRDF LUT Generator")
        .SetVertex({
            .module = shaderModule,
            .entry = "vs_main",
        })
        .SetFragment({
            .module = shaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetGFX()->GetSurfaceTextureFormat(Texture::Format::RG16Float, Texture::ColorSpace::Linear),
                },
            },
        })
        .Build();
}

Texture::Ref EnvBrdfLutGenerator::GenerateBrdfLut(const std::string& label) {
    auto texture = GetGFX()->CreateTexture()
        .SetLabel(label)
        .SetUsage(Texture::Usage::Texture | Texture::Usage::RenderTarget)
        .SetFormat(Texture::Format::RG16Float)
        .SetSize({ BRDF_LUT_SIZE, BRDF_LUT_SIZE, 1 })
        .Build();

    auto commandEncoder = GetGFX()->CreateCommandEncoder("Environment BRDF LUT Generator");
    commandEncoder->EncodeRenderPass({
        .label = "Environment BRDF LUT Generator",
        .colorAttachments = {
            RenderPass::ColorAttachment {
                .textureView = texture->CreateView().Build(),
                .clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0),
            },
        },
    }, [&](RenderEncoder* renderEncoder){
        renderEncoder->SetPipeline(shaderPipeline);
        renderEncoder->Draw(6);
    });
    auto commandBuffer = commandEncoder->Finish("Environment BRDF LUT Generator");
    GetGFX()->SubmitCommands(GetQueue(), commandBuffer);

    return texture;
}

static const std::string skyboxShaderCode = R"(
struct SkyboxUniforms {
    projMatrix: mat4x4f,
    viewMatrix: mat4x4f,
};
@group(0) @binding(0)
var<uniform> u_skybox: SkyboxUniforms;

@group(0) @binding(1) var u_cubeMap: texture_cube<f32>;
@group(0) @binding(2) var u_sampler: sampler;

struct VertexInput {
    @location(0) position: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) localPosition: vec3f,
};

fn removeTranslation(m: mat4x4f) -> mat4x4f {
    return mat4x4f(
        vec4f(m[0].xyz, 0.0),
        vec4f(m[1].xyz, 0.0),
        vec4f(m[2].xyz, 0.0),
        vec4f(0.0, 0.0, 0.0, 1.0),
    );
}

@vertex
fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = (u_skybox.projMatrix * removeTranslation(u_skybox.viewMatrix) * vec4f(in.position, 1.0)).xyww;
    out.localPosition = in.position;
    return out;
}

@fragment
fn fs_main(in: VertexOutput) -> @location(0) vec4f {
    var color = textureSampleLevel(u_cubeMap, u_sampler, in.localPosition, 7).rgb;

    return vec4f(color, 1.0);
}
)";

Skybox::Skybox(Texture::Ref cubeMap, Texture::Ref irradianceMap, Texture::Ref radianceMap, Texture::Ref brdfLut)
    : cubeMap(std::move(cubeMap)),
      irradianceMap(std::move(irradianceMap)),
      radianceMap(std::move(radianceMap)),
      brdfLut(std::move(brdfLut)) {
    auto bindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("Skybox")
        .AddEntry(0, {
            .type = BindSet::Layout::Entry::Type::Buffer,
            .buffer = {
                .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
                .minSize = sizeof(SkyboxUniforms),
            },
        })
        .AddEntry(1, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {
            .viewDimension = Texture::View::Dimension::Cube,
        }})
        .AddEntry(2, { .type = BindSet::Layout::Entry::Type::Sampler, .sampler = {} })
        .Build();

    uniformBuffer = GetGFX()->CreateBuffer()
        .SetLabel("Skybox Uniform Buffer")
        .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
        .SetSize(sizeof(SkyboxUniforms))
        .Build();

    bindSet = bindSetLayout->Instantiate()
        .SetLabel("Skybox")
        .SetEntry(0, { BindSet::Entry::Buffer {
            .buffer = uniformBuffer,
            .size = uniformBuffer->GetSize(),
        }})
        .SetEntry(1, { BindSet::Entry::TextureView {
            .textureView = this->cubeMap->CreateView()
                .SetDimension(Texture::View::Dimension::Cube)
                .SetArrayLayerCount(6)
                .Build(),
        }})
        .SetEntry(2, { BindSet::Entry::Sampler {
            .sampler = GetGFX()->CreateSampler()
                .SetLabel("Skybox")
                .SetAddressModeU(Sampler::AddressMode::Clamp)
                .SetAddressModeV(Sampler::AddressMode::Clamp)
                .SetAddressModeW(Sampler::AddressMode::Clamp)
                .Build(),
        }})
        .Build();

    auto pipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("Skybox")
        .AddBindSetLayout(bindSetLayout)
        .Build();

    Data::Shader shaderData {
        .language = SLGL::Data::Shader::Language::WGSL,
        .source = skyboxShaderCode,
    };
    auto shaderSource = GetGFX()->CreateShaderSource(shaderData);
    auto shaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Skybox")
        .SetSource(shaderSource)
        .Build();

    shaderPipeline = GetGFX()->CreateShaderRenderPipeline(pipelineLayout)
        .SetLabel("Skybox")
        .SetVertex({
            .module = shaderModule,
            .entry = "vs_main",
            .buffers = {
                ShaderPipeline::Render::Vertex::Buffer {
                    .stride = sizeof(glm::vec3),
                    .attributes = {
                        ShaderPipeline::Render::Vertex::Buffer::Attribute {
                            .format = SLGL::Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3,
                            .location = 0,
                        },
                    },
                },
            },
        })
        .SetDepthStencil({ })
        .SetMultisample({
            .count = GetMultisampleCount(),
        })
        .SetFragment({
            .module = shaderModule,
            .entry = "fs_main",
            .targets = {
                ShaderPipeline::Render::Fragment::ColorTarget {
                    .surfaceFormat = GetRenderTextureSurfaceFormat(),
                },
            },
        })
        .Build();

    positionBuffer = GetGFX()->CreateBuffer()
        .SetLabel("Skybox Position Buffer")
        .SetUsage(Buffer::Usage::Vertex | Buffer::Usage::Write)
        .SetSize(sizeof(cubeVertexPositions))
        .Build();
    positionBuffer->Write(GetQueue(), (void*)cubeVertexPositions);
}

Skybox Skybox::FromEquirectangularMap(const Texture::Ref& equirectangularMap) {
    CubeMapConverter converter;
    auto cubeMap = converter.ConvertEquirectangularToCubeMap(equirectangularMap);
    auto irradianceMap = converter.GenerateIrradianceMap(cubeMap);
    auto radianceMap = converter.GenerateRadianceMap(cubeMap);

    EnvBrdfLutGenerator brdfLutGenerator;
    auto brdfLut = brdfLutGenerator.GenerateBrdfLut();

    return Skybox(cubeMap, irradianceMap, radianceMap, brdfLut);
}

Skybox Skybox::FromEquirectangularMap(const Data::Image& image) {
    auto texture = GetGFX()->CreateTexture()
        .SetLabel("Skybox Equirectangular Map")
        .SetSize({ image.width, image.height, 1 })
        .SetFormat(Texture::Format::RGBA16Float)
        .SetUsage(Texture::Usage::Write | Texture::Usage::Texture)
        .Build();
    texture->Write(GetQueue(), image.data.get(), image.size);
    return FromEquirectangularMap(texture);
}

void Skybox::Draw(glm::mat4 projMatrix, glm::mat4 viewMatrix, RenderEncoder* renderEncoder) {
    SkyboxUniforms uniforms {
        .projMatrix = projMatrix,
        .viewMatrix = viewMatrix,
    };
    uniformBuffer->Write(GetQueue(), &uniforms);

    renderEncoder->SetPipeline(shaderPipeline);
    renderEncoder->SetBindSet(0, bindSet);
    renderEncoder->SetVertexBuffer(0, positionBuffer);
    renderEncoder->Draw(36);
}

Texture::Ref Skybox::GetIrradianceMap() const { return irradianceMap; }
Texture::Ref Skybox::GetRadianceMap() const { return radianceMap; }
Texture::Ref Skybox::GetBrdfLut() const { return brdfLut; }

int Skybox::GetRadianceMipMapCount() {
    return RADIANCE_MIPMAP_COUNT;
}

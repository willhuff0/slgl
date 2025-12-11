#include <slgl/runtime/game/Terrain.hpp>

#include <slgl/runtime/game/Provider.hpp>
#include <slgl/runtime/game/deps/FastNoiseLite.h>
#include <slgl/runtime/game/TempImageLoader.hpp>
#include <slgl/runtime/game/utils/Paths.hpp>

static const std::string terrainShaderCode = R"(
struct GlobalUniforms {
    viewProjMatrix: mat4x4f,
    viewPosition: vec4f,
    radianceMipMapCount: u32,
};
@group(0) @binding(0)
var<uniform> u_global: GlobalUniforms;

@group(0) @binding(1) var u_environmentSampler: sampler;
@group(0) @binding(2) var u_irradianceMap: texture_cube<f32>;
@group(0) @binding(3) var u_radianceMap: texture_cube<f32>;
@group(0) @binding(4) var u_brdfLut: texture_2d<f32>;

struct TerrainUniforms {
    tiling: f32,
    ambientIntensity: f32,
};
@group(1) @binding(0) var<uniform> terrainUniforms: TerrainUniforms;

@group(1) @binding(1) var u_albedoTexture: texture_2d<f32>;
@group(1) @binding(2) var u_metallicTexture: texture_2d<f32>;
@group(1) @binding(3) var u_roughnessTexture: texture_2d<f32>;
@group(1) @binding(4) var u_aoTexture: texture_2d<f32>;
@group(1) @binding(5) var u_sampler: sampler;

struct ChunkUniforms {
    modelMatrix: mat4x4f,
};
@group(2) @binding(0) var<uniform> chunkUniforms: ChunkUniforms;

struct VertexInput {
    @location(0) position: vec3f,
    @location(1) normal: vec3f,
};

struct VertexOutput {
    @builtin(position) position: vec4f,
    @location(0) worldPosition: vec4f,
    @location(1) normal: vec3f,
};

@vertex
fn vs_main(input: VertexInput) -> VertexOutput {
    var output: VertexOutput;
    output.position = u_global.viewProjMatrix * chunkUniforms.modelMatrix * vec4f(input.position, 1.0);
    output.worldPosition = chunkUniforms.modelMatrix * vec4f(input.position, 1.0);
    output.normal = input.normal;
    return output;
}

const PI = 3.14159265359;

fn FresnelSchlick(cosTheta: f32, F0: vec3f) -> vec3f {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

fn FresnelSchlickRoughness(cosTheta: f32, F0: vec3f, roughness: f32) -> vec3f {
    return F0 + (max(vec3f(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

fn DistributionGGX(N: vec3f, H: vec3f, roughness: f32) -> f32 {
    let a = roughness * roughness;
    let a2 = a * a;
    let NdotH = max(dot(N, H), 0.0);
    let NdotH2 = NdotH * NdotH;

    let num = a2;
    var denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

fn GeometrySchlickGGX(NdotV: f32, roughness: f32) -> f32 {
    let r = (roughness + 1.0);
    let k = (r * r) / 8.0;

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

@fragment
fn fs_main(input: VertexOutput) -> @location(0) vec4f {
    let texCoord = vec2f(input.worldPosition.x, input.worldPosition.z) * terrainUniforms.tiling;
    let albedo = textureSample(u_albedoTexture, u_sampler, texCoord).rgb;
    let metallic = textureSample(u_metallicTexture, u_sampler, texCoord).r;
    let roughness = textureSample(u_roughnessTexture, u_sampler, texCoord).r;
    let ao = textureSample(u_aoTexture, u_sampler, texCoord).r;

    var F0 = vec3f(0.04);
    F0 = mix(F0, albedo, metallic);

    let N = normalize(input.normal);
    let V = normalize(u_global.viewPosition.xyz - input.worldPosition.xyz);
    let R = reflect(-V, N);

    var Lo = vec3f(0.0);

// Directional Light
    {
        let L = normalize(vec3f(0.5, 1.0, 0.5));
        let H = normalize(V + L);

        let radiance = vec3f(1.0, 0.95, 0.9) * 1.5;

        let F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        let NDF = DistributionGGX(N, H, roughness);
        let G = GeometrySmith(N, V, L, roughness);

        let numerator = NDF * G * F;
        let denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        let specular = numerator / denominator;

        let kS = F;
        var kD = vec3f(1.0) - kS;
        kD *= (1.0 - metallic);

        let NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    var ambient = vec3f(0.0);

// Environment
    {
        let F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        let kS = F;
        var kD = 1.0 - kS;
        kD *= 1.0 - metallic;

        let irradiance = textureSample(u_irradianceMap, u_environmentSampler, N).rgb;
        let diffuse = irradiance * albedo;

        let prefilteredColor = textureSampleLevel(u_radianceMap, u_environmentSampler, R, roughness * f32(u_global.radianceMipMapCount)).rgb;
        let envBrdf = textureSample(u_brdfLut, u_environmentSampler, vec2f(max(dot(N, V), 0.0), roughness)).rg;
        let specular = prefilteredColor * (F * envBrdf.x + envBrdf.y);

        ambient = (kD * diffuse + specular) * ao * terrainUniforms.ambientIntensity;
    }

    var color = ambient + Lo;

//    color = 1.0 - exp(-color * 1.2);
//    color = pow(color, vec3f(1.0 / 2.2));

    return vec4f(color, 1.0);
}
)";

const int viewDistance = 32;
const float cellSize = 1.0f;
const int chunkSize = 32;
const int indexCount = chunkSize * chunkSize * 6;

Chunk::Chunk(GridPos pos, const BindSet::Layout::Ref& bindSetLayout,
             const std::vector<glm::vec3>& positions,
             const std::vector<glm::vec3>& normals,
             const std::vector<uint32_t>& indices) {
    std::string chunkName = "Chunk (" + std::to_string(pos.x) + "," + std::to_string(pos.z) + ")";

    positionBuffer = GetGFX()->CreateBuffer()
            .SetLabel(chunkName + " Position Buffer")
            .SetSize(positions.size() * sizeof(glm::vec3))
            .SetUsage(Buffer::Usage::Vertex | Buffer::Usage::Write)
            .Build();
    positionBuffer->Write(GetQueue(), (void*)positions.data());

    normalBuffer = GetGFX()->CreateBuffer()
            .SetLabel(chunkName + " Normal Buffer")
            .SetSize(normals.size() * sizeof(glm::vec3))
            .SetUsage(Buffer::Usage::Vertex | Buffer::Usage::Write)
            .Build();
    normalBuffer->Write(GetQueue(), (void*)normals.data());

    indexBuffer = GetGFX()->CreateBuffer()
        .SetLabel(chunkName + " Index Buffer")
        .SetSize(indices.size() * sizeof(uint32_t))
        .SetUsage(Buffer::Usage::Index | Buffer::Usage::Write)
        .Build();
    indexBuffer->Write(GetQueue(), (void*)indices.data());

    auto translation = glm::translate(glm::identity<glm::mat4>(), glm::vec3(
        (float)(pos.x * chunkSize) * cellSize,
        0.0f,
        (float)(-pos.z * chunkSize) * cellSize
    ));

    auto uniformBuffer = GetGFX()->CreateBuffer()
        .SetLabel(chunkName + " Uniform Buffer")
        .SetSize(sizeof(ChunkUniforms))
        .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
        .Build();
    ChunkUniforms uniforms {
        .modelMatrix = glm::translate(glm::identity<glm::mat4>(), glm::vec3(
            (float)(pos.x * chunkSize) * cellSize,
            0.0f,
            (float)(-pos.z * chunkSize) * cellSize
        )),
    };
    uniformBuffer->Write(GetQueue(), &uniforms);

    bindSet = bindSetLayout->Instantiate()
        .SetLabel(chunkName + " Bind Set")
        .SetEntry(0, { BindSet::Entry::Buffer {
            .buffer = uniformBuffer,
            .size = uniformBuffer->GetSize(),
        }})
        .Build();
}

void Chunk::Draw(SLGL::Graphics::RenderEncoder* renderEncoder) {
    renderEncoder->SetIndexBuffer(indexBuffer, RenderEncoder::IndexFormat::Uint32);
    renderEncoder->SetVertexBuffer(0, positionBuffer);
    renderEncoder->SetVertexBuffer(1, normalBuffer);
    renderEncoder->SetBindSet(2, bindSet);
    renderEncoder->DrawIndexed(indexCount);
}

Terrain::Terrain(int seed) : seed(seed) {
    chunkBindSetLayout = GetGFX()->CreateBindSetLayout()
            .SetLabel("Terrain Chunk")
            .AddEntry(0, {
                .type = BindSet::Layout::Entry::Type::Buffer,
                .buffer = {
                    .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
                    .minSize = sizeof(ChunkUniforms),
                },
            })
            .Build();

    auto terrainBindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("Terrain")
        .AddEntry(0, {
            .type = BindSet::Layout::Entry::Type::Buffer,
            .buffer = {
                .type = BindSet::Layout::Entry::Buffer::Type::Uniform,
                .minSize = sizeof(TerrainUniforms),
            },
        })
        .AddEntry(1, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {} })
        .AddEntry(2, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {} })
        .AddEntry(3, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {} })
        .AddEntry(4, { .type = BindSet::Layout::Entry::Type::Texture, .texture = {} })
        .AddEntry(5, { .type = BindSet::Layout::Entry::Type::Sampler })
        .Build();

    auto terrainUniformBuffer = GetGFX()->CreateBuffer()
        .SetLabel("Terrain Uniform Buffer")
        .SetSize(sizeof(TerrainUniforms))
        .SetUsage(Buffer::Usage::Uniform | Buffer::Usage::Write)
        .Build();
    TerrainUniforms terrainUniforms {
        .tiling = 0.125f,
        .ambientIntensity = 2.0f,
    };
    terrainUniformBuffer->Write(GetQueue(), &terrainUniforms);

    auto albedoTexture = GetGFX()->CreateTexture("Terrain Albedo", GetQueue(), LoadImage(TERRAIN_ALBEDO_TEXTURE_PATH, 3, Data::Image::ColorSpace::SRGB));
    auto metallicTexture = GetGFX()->CreateTexture("Terrain Metallic", GetQueue(), LoadImage(TERRAIN_METALLIC_TEXTURE_PATH));
    auto roughnessTexture = GetGFX()->CreateTexture("Terrain Roughness", GetQueue(), LoadImage(TERRAIN_ROUGHNESS_TEXTURE_PATH));
    auto aoTexture = GetGFX()->CreateTexture("Terrain AO", GetQueue(), LoadImage(TERRAIN_AO_TEXTURE_PATH));
    auto sampler = GetGFX()->CreateSampler()
        .SetLabel("Terrain Sampler")
        .SetAddressModeU(Sampler::AddressMode::Repeat)
        .SetAddressModeV(Sampler::AddressMode::Repeat)
        .SetAddressModeW(Sampler::AddressMode::Repeat)
        .Build();

    bindSet = terrainBindSetLayout->Instantiate()
        .SetLabel("Terrain")
        .SetEntry(0, { BindSet::Entry::Buffer {
            .buffer = terrainUniformBuffer,
            .size = terrainUniformBuffer->GetSize(),
        }})
        .SetEntry(1, { BindSet::Entry::TextureView {
            .textureView = albedoTexture->CreateView().Build(),
        }})
        .SetEntry(2, { BindSet::Entry::TextureView {
            .textureView = metallicTexture->CreateView().Build(),
        }})
        .SetEntry(3, { BindSet::Entry::TextureView {
            .textureView = roughnessTexture->CreateView().Build(),
        }})
        .SetEntry(4, { BindSet::Entry::TextureView {
            .textureView = aoTexture->CreateView().Build(),
        }})
        .SetEntry(5, { BindSet::Entry::Sampler {
            .sampler = sampler,
        }})
        .Build();

    shaderPipelineLayout = GetGFX()->CreateShaderPipelineLayout()
        .SetLabel("Terrain")
        .AddBindSetLayout(GetGlobalBindSetLayout())
        .AddBindSetLayout(terrainBindSetLayout)
        .AddBindSetLayout(chunkBindSetLayout)
        .Build();

    Data::Shader shaderData {
        .language = SLGL::Data::Shader::Language::WGSL,
        .source = terrainShaderCode,
    };
    auto shaderSource = GetGFX()->CreateShaderSource(shaderData);
    auto shaderModule = GetGFX()->CreateShaderModule()
        .SetLabel("Terrain")
        .SetSource(shaderSource)
        .Build();

    shaderPipeline = GetGFX()->CreateShaderRenderPipeline(shaderPipelineLayout)
        .SetLabel("Terrain")
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
                ShaderPipeline::Render::Vertex::Buffer {
                    .stride = sizeof(glm::vec3),
                    .attributes = {
                        ShaderPipeline::Render::Vertex::Buffer::Attribute {
                            .format = SLGL::Graphics::ShaderPipeline::Render::Vertex::Buffer::Attribute::Format::Float32x3,
                            .location = 1,
                        },
                    },
                },
            },
        })
        .SetPrimitive({
            .cullMode = ShaderPipeline::Render::Primitive::CullMode::None,
        })
        .SetDepthStencil({ })
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
}

void Terrain::Tick(glm::vec3 playerPos) {
    GridPos playerChunkPos = {
        .x = (int)std::floor(playerPos.x / (chunkSize * cellSize)),
        .z = (int)std::floor(-playerPos.z / (chunkSize * cellSize)),
    };

    std::unordered_set<GridPos> chunksToUnload(visibleChunks);
    std::unordered_set<GridPos> chunksToLoad;
    for (int cx = -viewDistance; cx < viewDistance; cx++) {
        for (int cz = -viewDistance; cz < viewDistance; cz++) {
            if (cx * cx + cz * cz > viewDistance * viewDistance) continue;

            int x = cx + playerChunkPos.x;
            int z = cz + playerChunkPos.z;

            auto iter = chunksToUnload.find({ x, z });
            if (iter == chunksToUnload.end()) {
                chunksToLoad.emplace(x, z);
            } else {
                chunksToUnload.erase(iter);
            }
        }
    }

    for (const auto& chunkPos: chunksToUnload) {
        visibleChunks.erase(chunkPos);

        //chunks.at(chunkPos).Unload();
    }
    for (const auto& chunkPos: chunksToLoad) {
        visibleChunks.emplace(chunkPos);

        auto iter = chunks.find(chunkPos);
        if (iter == chunks.end()) {
            generateChunk(chunkPos);
        } else {
            //iter->second.Load();
        }
    }
}

void Terrain::Draw(RenderEncoder* renderEncoder) {
    renderEncoder->SetPipeline(shaderPipeline);
    renderEncoder->SetBindSet(1, bindSet);
    for (const auto& chunkPos : visibleChunks) {
        chunks.at(chunkPos).Draw(renderEncoder);
    }
}

void Terrain::generateChunk(GridPos pos) {
    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    noise.SetFractalType(FastNoiseLite::FractalType_Ridged);
    noise.SetFractalOctaves(6);
    noise.SetFrequency(0.0015);
    noise.SetSeed(seed);

    const int heightsStride = chunkSize + 3;
    float heights[heightsStride][heightsStride];

    for (int localX = -1; localX < heightsStride - 1; localX++) {
        for (int localZ = -1; localZ < heightsStride - 1; localZ++) {
            float worldX = (float)(pos.x * chunkSize + localX) * cellSize;
            float worldZ = (float)(-pos.z * chunkSize - localZ) * cellSize;

            heights[localX + 1][localZ + 1] = noise.GetNoise(worldX, worldZ) * 24.0f;
        }
    }

    const int meshStride = chunkSize + 1;
    const int vertexCount = meshStride * meshStride;

    std::vector<glm::vec3> positions;
    positions.reserve(vertexCount);
    for (int localX = 0; localX < meshStride; localX++) {
        for (int localZ = 0; localZ < meshStride; localZ++) {
            const auto& height = heights[localX + 1][localZ + 1];
            positions.emplace_back((float)localX * cellSize, height, (float)-localZ * cellSize);
        }
    }

    std::vector<uint32_t> indices;
    indices.reserve(indexCount);
    for (int localX = 0; localX < chunkSize; localX++) {
        for (int localZ = 0; localZ < chunkSize; localZ++) {
            unsigned short i0 = (localX + 0) * meshStride + (localZ + 0);
            unsigned short i1 = (localX + 1) * meshStride + (localZ + 0);
            unsigned short i2 = (localX + 1) * meshStride + (localZ + 1);
            unsigned short i3 = (localX + 0) * meshStride + (localZ + 1);

            indices.emplace_back(i0);
            indices.emplace_back(i1);
            indices.emplace_back(i2);
            indices.emplace_back(i2);
            indices.emplace_back(i3);
            indices.emplace_back(i0);
        }
    }

    std::vector<glm::vec3> normals;
    normals.resize(vertexCount);
    for (int localX = -1; localX < heightsStride - 2; localX++) {
        for (int localZ = -1; localZ < heightsStride - 2; localZ++) {
            const auto& v0 = glm::vec3((float)(localX + 0) * cellSize, heights[localX + 1][localZ + 1], (float)(-localZ + 0) * cellSize);
            const auto& v1 = glm::vec3((float)(localX + 1) * cellSize, heights[localX + 2][localZ + 1], (float)(-localZ + 0) * cellSize);
            const auto& v2 = glm::vec3((float)(localX + 1) * cellSize, heights[localX + 2][localZ + 2], (float)(-localZ - 1) * cellSize);
            const auto& v3 = glm::vec3((float)(localX + 0) * cellSize, heights[localX + 1][localZ + 2], (float)(-localZ - 1) * cellSize);

            glm::vec3 n1 = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            if (localX >= 0 && localZ >= 0) normals[(localX + 0) * meshStride + (localZ + 0)] += n1;
            if (localX + 1 < meshStride && localZ >= 0) normals[(localX + 1) * meshStride + (localZ + 0)] += n1;
            if (localX + 1 < meshStride && localZ + 1 < meshStride) normals[(localX + 1) * meshStride + (localZ + 1)] += n1;

            glm::vec3 n2 = glm::normalize(glm::cross(v3 - v2, v0 - v2));
            if (localX + 1 < meshStride && localZ + 1 < meshStride) normals[(localX + 1) * meshStride + (localZ + 1)] += n2;
            if (localX >= 0 && localZ + 1 < meshStride) normals[(localX + 0) * meshStride + (localZ + 1)] += n2;
            if (localX >= 0 && localZ >= 0) normals[(localX + 0) * meshStride + (localZ + 0)] += n2;
        }
    }

    for (auto& normal: normals) {
        normal = glm::normalize(normal);
    }

    chunks.emplace(std::piecewise_construct, std::forward_as_tuple(pos), std::forward_as_tuple(pos, chunkBindSetLayout, positions, normals, indices));
}
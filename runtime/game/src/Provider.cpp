#include <slgl/runtime/game/Provider.hpp>

#include <slgl/runtime/game/ecs/moodycamel/MoodycamelQueue.hpp>

static Platform::Backend* platformBackend;
static Graphics::Backend* graphicsBackend;
static Graphics::Queue* graphicsQueue;

static uint32_t graphicsRenderTextureFormat;
static uint32_t graphicsSurfaceFormat;

static Graphics::BindSet::Layout::Ref globalBindSetLayout;
static Graphics::Buffer::Ref globalUniformsBuffer;
static Graphics::BindSet::Ref globalBindSet;
static Graphics::Sampler::Ref environmentSampler;

static const auto renderTextureFormat = Graphics::Texture::Format::RGBA16Float;

static std::shared_ptr<Decs> decs;

void SetupProvider(Platform::Backend* platform, Graphics::Backend* gfx, uint32_t surfaceFormat) {
    platformBackend = platform;
    graphicsBackend = gfx;
    graphicsQueue = gfx->GetQueueFamilies()[0].second[0];
    graphicsRenderTextureFormat = gfx->GetSurfaceTextureFormat(renderTextureFormat, Graphics::Texture::ColorSpace::Linear);
    graphicsSurfaceFormat = surfaceFormat;

    globalBindSetLayout = GetGFX()->CreateBindSetLayout()
        .SetLabel("Global")
        .AddEntry(0, {
            .type = Graphics::BindSet::Layout::Entry::Type::Buffer,
            .buffer = {
                .type = Graphics::BindSet::Layout::Entry::Buffer::Type::Uniform,
                .minSize = sizeof(GlobalUniforms),
            },
        })
        .AddEntry(1, {
            .type = Graphics::BindSet::Layout::Entry::Type::Sampler,
            .sampler = { },
        })
        .AddEntry(2, {
            .type = Graphics::BindSet::Layout::Entry::Type::Texture,
            .texture = {
                .viewDimension = Graphics::Texture::View::Dimension::Cube,
            }
        })
        .AddEntry(3, {
            .type = Graphics::BindSet::Layout::Entry::Type::Texture,
            .texture = {
                .viewDimension = Graphics::Texture::View::Dimension::Cube,
            }
        })
        .AddEntry(4, {
            .type = Graphics::BindSet::Layout::Entry::Type::Texture,
            .texture = { }
        })
        .Build();

    globalUniformsBuffer = GetGFX()->CreateBuffer()
        .SetLabel("Global Uniform Buffer")
        .SetSize(sizeof(GlobalUniforms))
        .SetUsage(Graphics::Buffer::Usage::Uniform | Graphics::Buffer::Usage::Write)
        .Build();

    environmentSampler = GetGFX()->CreateSampler()
        .SetLabel("Global Environment Sample")
        .SetAddressModeU(Graphics::Sampler::AddressMode::Clamp)
        .SetAddressModeV(Graphics::Sampler::AddressMode::Clamp)
        .SetAddressModeW(Graphics::Sampler::AddressMode::Clamp)
        .SetMinFilter(Graphics::Sampler::FilterMode::Linear)
        .SetMagFilter(Graphics::Sampler::FilterMode::Linear)
        .Build();

    decs = Decs::Create(std::make_unique<MoodycamelQueue<std::function<void()>>>());
}

void SetGlobalUniforms(GlobalUniforms* uniforms) {
    globalUniformsBuffer->Write(graphicsQueue, uniforms);
}

void SetSkybox(const Graphics::Texture::Ref& irradianceMap,
               const Graphics::Texture::Ref& radianceMap,
               const Graphics::Texture::Ref& brdfLut) {
    globalBindSet = globalBindSetLayout->Instantiate()
        .SetEntry(0, { Graphics::BindSet::Entry::Buffer {
            .buffer = globalUniformsBuffer,
            .size = globalUniformsBuffer->GetSize(),
        }})
        .SetEntry(1, { Graphics::BindSet::Entry::Sampler {
            .sampler = environmentSampler,
        }})
        .SetEntry(2, { Graphics::BindSet::Entry::TextureView {
            .textureView = irradianceMap->CreateView()
                .SetLabel("Global Irradiance CubeMap")
                .SetDimension(Graphics::Texture::View::Dimension::Cube)
                .SetArrayLayerCount(6)
                .Build(),
        }})
        .SetEntry(3, { Graphics::BindSet::Entry::TextureView {
            .textureView = radianceMap->CreateView()
                .SetLabel("Global Radiance CubeMap")
                .SetDimension(Graphics::Texture::View::Dimension::Cube)
                .SetArrayLayerCount(6)
                .Build(),
        }})
        .SetEntry(4, { Graphics::BindSet::Entry::TextureView {
            .textureView = brdfLut->CreateView()
                .SetLabel("Global Environment BRDF LUT")
                .Build(),
        }})
        .Build();
}

void BindGlobals(Graphics::RenderEncoder* renderEncoder) {
    renderEncoder->SetBindSet(0, globalBindSet);
}

Graphics::BindSet::Layout::Ref GetGlobalBindSetLayout() { return globalBindSetLayout; }

Platform::Backend* GetPlatform() { return platformBackend; }
Graphics::Backend* GetGFX() { return graphicsBackend; }
Graphics::Queue* GetQueue() { return graphicsQueue; }
Graphics::Texture::Format GetRenderTextureFormat() { return renderTextureFormat; }
uint32_t GetRenderTextureSurfaceFormat() { return graphicsRenderTextureFormat; }
uint32_t GetDisplaySurfaceFormat() { return graphicsSurfaceFormat; }

uint32_t GetMultisampleCount() { return 4; }

Decs* GetDecs() { return decs.get(); }

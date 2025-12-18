#include <slgl/runtime/game/Game.hpp>

#include <slgl/runtime/game/Provider.hpp>
#include <slgl/runtime/game/utils/Paths.hpp>
#include <slgl/runtime/game/TempImageLoader.hpp>

#include <slgl/runtime/surfaces/Surfaces.hpp>

#include <decs/Decs.hpp>

#include <utility>

Game::Game(Platform::Backend* platform, Platform::Window::Ref window, Graphics::Backend* gfx, Graphics::Surface::Ref surface)
    : platform(platform),
      window(std::move(window)),
      gfx(gfx),
      surface(std::move(surface)),
      flyCamera(static_cast<GLFWwindow*>(this->window->GetNativeHandle()), glm::vec3(0.01f, 26.0f, 0.01f)) {
    postProcessing = std::make_unique<PostProcessing>();
    skybox = std::make_unique<Skybox>(Skybox::FromEquirectangularMap(LoadHDRI(SKYBOX_HDRI_PATH)));
    terrain = std::make_unique<Terrain>(6767);

    SetSkybox(skybox->GetIrradianceMap(), skybox->GetRadianceMap(), skybox->GetBrdfLut());

    meshRenderer = std::make_unique<MeshRenderer>();
}

void Game::MainLoop() {
    // Tick

    flyCamera.Tick();
    terrain->Tick(flyCamera.GetPos());

    GetDecs()->ExecuteDeferredFunctions();
    GetDecs()->IterateSystems();

    // Draw

    // Update render textures if surface size changed
    if (surfaceSize.x != surface->GetWidth() || surfaceSize.y != surface->GetHeight()) {
        surfaceSize = glm::ivec3(surface->GetWidth(), surface->GetHeight(), 1);

        depthTextureView = GetGFX()->CreateTexture()
            .SetLabel("Depth Texture")
            .SetUsage(Texture::Usage::RenderTarget)
            .SetFormat(Texture::Format::Depth24Plus)
            .SetSampleCount(GetMultisampleCount())
            .SetSize(surfaceSize)
            .Build()->CreateView().Build();

        multisampleTextureView = GetGFX()->CreateTexture()
            .SetLabel("Pre Post Processing Multisample Render Texture")
            .SetUsage(Texture::Usage::RenderTarget)
            .SetFormat(GetRenderTextureFormat())
            .SetSampleCount(GetMultisampleCount())
            .SetSize(surfaceSize)
            .Build()->CreateView().Build();

        postProcessing->ResizeRenderTexture(glm::ivec2(surfaceSize.x, surfaceSize.y));
    }

    GlobalUniforms globalUniforms {
        .viewProjMatrix = flyCamera.GetProjMat() * flyCamera.GetViewMat(),
        .viewPosition = glm::vec4(flyCamera.GetPos(), 1.0f),
        .radianceMipMapCount = (uint32_t)Skybox::GetRadianceMipMapCount(),
    };
    SetGlobalUniforms(&globalUniforms);

    auto commandEncoder = GetGFX()->CreateCommandEncoder("Main");

    // Main Render Pass
    auto postProcessingInputRenderTextureView = postProcessing->GetPostProcessingInputTextureView();
    commandEncoder->EncodeRenderPass({
        .label = "Main",
        .colorAttachments = {
            RenderPass::ColorAttachment {
                .textureView = multisampleTextureView,
                .resolveTextureView = postProcessingInputRenderTextureView,
                .clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0),
            },
        },
        .depthStencilAttachment = {
            RenderPass::DepthStencilAttachment {
                .textureView = depthTextureView,
            },
        },
    }, [&](RenderEncoder* renderEncoder){
        BindGlobals(renderEncoder);
        terrain->Draw(renderEncoder);
        skybox->Draw(flyCamera.GetProjMat(), flyCamera.GetViewMat(), renderEncoder);
    });

    // Post Processing Render Pass
    auto surfaceTextureView = surface->GetCurrentTextureView();
    postProcessing->PostProcessingRenderPass(commandEncoder, surfaceTextureView);

    auto commandBuffer = commandEncoder->Finish("Main");
    GetGFX()->SubmitCommands(GetQueue(), commandBuffer);
}

#pragma once

#include <slgl/runtime/platform/Platform.hpp>
#include <slgl/runtime/graphics/Graphics.hpp>

#include <decs/Decs.hpp>

using namespace SLGL;

struct alignas(16) GlobalUniforms {
    glm::mat4 viewProjMatrix;
    glm::vec4 viewPosition;
    uint32_t radianceMipMapCount;
};

void SetupProvider(Platform::Backend* platform, Graphics::Backend* gfx, uint32_t surfaceFormat);

void SetGlobalUniforms(GlobalUniforms* uniforms);
void SetSkybox(const Graphics::Texture::Ref& irradianceMap,
               const Graphics::Texture::Ref& radianceMap,
               const Graphics::Texture::Ref& brdfLuts);

void BindGlobals(Graphics::RenderEncoder* renderEncoder);

Graphics::BindSet::Layout::Ref GetGlobalBindSetLayout();

Platform::Backend* GetPlatform();
Graphics::Backend* GetGFX();
Graphics::Queue* GetQueue();
Graphics::Texture::Format GetRenderTextureFormat();
uint32_t GetRenderTextureSurfaceFormat();
uint32_t GetDisplaySurfaceFormat();

uint32_t GetMultisampleCount();

Decs* GetDecs();

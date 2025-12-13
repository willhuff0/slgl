#pragma once

#include <slgl/runtime/platform/Platform.hpp>
#include <slgl/runtime/graphics/Graphics.hpp>

#include "FlyCamera.hpp"
#include "PostProcessing.hpp"
#include "Terrain.hpp"
#include "Skybox.hpp"

#include <memory>

using namespace SLGL;

class Game {
public:
    Game(Platform::Backend* platform, Platform::Window::Ref window, Graphics::Backend* gfx, Graphics::Surface::Ref surface);

    void MainLoop();

private:
    Platform::Backend* platform;
    Platform::Window::Ref window;
    Graphics::Backend* gfx;

    Graphics::Surface::Ref surface;
    glm::ivec3 surfaceSize = glm::ivec3(0);
    Graphics::Texture::View::Ref depthTextureView;
    Graphics::Texture::View::Ref multisampleTextureView;

    FlyCamera flyCamera;
    std::unique_ptr<PostProcessing> postProcessing = nullptr;
    std::unique_ptr<Skybox> skybox = nullptr;
    std::unique_ptr<Terrain> terrain = nullptr;
};

#include <slgl/runtime/platform/Platform.hpp>
#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/runtime/wsi/WSI.hpp>

#include <slgl/runtime/game/Provider.hpp>
#include <slgl/runtime/game/Game.hpp>

using namespace SLGL;

int main() {
    Platform::Backend* platform = Platform::GetBackend(Platform::BackendType::GLFW);
    auto window = platform->CreateWindow(1920, 1080, "Hello, SLGL!");

    auto extensions = WSI::GetRequiredExtensions(platform);

    Graphics::Backend* gfx = Graphics::GetBackend(extensions);
    auto surface = WSI::CreateSurface(gfx, window, true);

    SetupProvider(platform, gfx, surface->GetFormat());
    Game game(platform, window, gfx, surface);
    while (!window->ShouldClose()) {
        platform->ProcessEvents();
        game.MainLoop();
        gfx->Tick();
        surface->Present();
    }
}
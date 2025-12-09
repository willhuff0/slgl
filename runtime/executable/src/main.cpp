#include <slgl/runtime/platform/Platform.hpp>
#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/runtime/surfaces/Surfaces.hpp>

#include <slgl/runtime/game/Provider.hpp>
#include <slgl/runtime/game/Game.hpp>

using namespace SLGL;

int main() {
    Platform::Backend* platform = Platform::GetBackend(Platform::BackendType::GLFW);
    auto window = platform->CreateWindow(1920, 1080, "Hello, SLGL!");

    Graphics::Backend* gfx = Graphics::GetBackend();
    auto surface = Surfaces::CreateSurface(gfx, window, true);

    SetupProvider(platform, gfx, surface->GetFormat());
    Game game(platform, window, gfx, surface);
    while (!window->ShouldClose()) {
        platform->ProcessEvents();
        game.MainLoop();
        gfx->Tick();
        surface->Present();
    }
}
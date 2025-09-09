#include <slgl/runtime/platform/Platform.hpp>
#include <slgl/runtime/graphics/Graphics.hpp>
#include <slgl/runtime/surfaces/Surfaces.hpp>

using namespace SLGL;

int main() {
    Platform::Backend* platform = Platform::GetBackend();
    auto window = platform->CreateWindow(800, 600, "Hello, SLGL!");

    Graphics::Backend* gfx = Graphics::GetBackend();
    auto surface = Surfaces::CreateSurface(gfx, window, true);

    while (!window->ShouldClose()) {
        platform->ProcessEvents();
        gfx->Tick();
    }
}
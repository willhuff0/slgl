# SLGL

SLGL is a modular C++20 project that provides a thin runtime for windowing, GPU abstraction, and swapchain/surface management. The repository currently focuses on a WebGPU graphics backend with SDL3/GLFW platform backends and a minimal executable that exercises the stack.

## Status Overview
- Platform backends: SDL3 (default ON) and GLFW (optional) implemented.
- Graphics backend: WebGPU implemented (Dawn or wgpu-native distribution). DX12/Metal/Vulkan are placeholders in the abstraction.
- Surfaces: Pairings implemented for SDL3↔WebGPU and GLFW↔WebGPU.
- GPU resource builders and encoders implemented (buffers, textures, samplers, shader modules/pipelines, command encoders/buffers, render encoders).
- Common data: Image and Shader types, with TGA writer for debugging; simple Observer pattern utility.
- Scene/Physics/Simulation/Resource Reader: present as stubs.

## Design
- Interface-first modularity: `Platform::Backend`, `Platform::Window`, `Graphics::Backend`, and `Graphics::Surface` provide stable interfaces. Backends plug in via CMake options without altering user code.
- Builder pattern for GPU objects: Backends provide `Create***()` builders with consistent options across APIs; keeps call sites simple.
- Compile-time backend selection: CMake options gate features (`SLGL_PLATFORM_BACKEND_*`, `SLGL_GRAPHICS_BACKEND_*`). Users can select from selected backends at runtime or have slgl pick automatically.
- Explicit surface coupling: Surface creation bridges platform windows to graphics surfaces through small pairing modules (e.g., `GLFW_WebGPU`), keeping backend-specific interop isolated.
- Event propagation via observer: Windows notify size changes to surfaces using a tiny observer utility, decoupling platform from graphics.

## Structure
- `common/` shared types/utilities (`data/`, `math/`, `utils/`).
- `runtime/` high-level modules (`platform/`, `graphics/`, `surfaces/`, `scene/`, `simulation/`, `physics/`, `resource_reader/`).
- `runtime/executable/` entry point, links `SLGL::COMMON` and `SLGL::RUNTIME`.
- `generator/` offline asset processing (not implemented yet)

## Build and Run
Example: Debug build, SDL3 + WebGPU (default)
- Configure: `cmake -S . -B build`
- Build: `cmake --build build`
- Run: `./build/runtime/executable/SLGL_RUNTIME_EXECUTABLE`

Toggle backends
- GLFW platform: add `-DSLGL_PLATFORM_BACKEND_GLFW=ON -DSLGL_PLATFORM_BACKEND_SDL3=OFF`
- WebGPU implementation: `-DSLGL_WEBGPU_BACKEND=DAWN|WGPU`.

Note: First configure downloads dependencies (network required). The WebGPU distribution target copies the runtime library next to the executable.

## Minimal Usage Flow
```
Platform::Backend* platform = Platform::GetBackend();
auto window = platform->CreateWindow(800, 600, "Hello, SLGL!");

Graphics::Backend* gfx = Graphics::GetBackend();
auto surface = Surfaces::CreateSurface(gfx, window, /*vsync=*/true);

while (!window->ShouldClose()) {
  platform->ProcessEvents();
  gfx->Tick();
}
```

## Graphics API Example
```
using namespace SLGL::Graphics;

Buffer::Ref buffer = gfx->CreateBuffer()
    .SetLabel("My Buffer")
    .SetSize(1024 * 1024)
    .SetUsage(Buffer::Usage::Write | Buffer::Usage::Vertex)
    .Build();

buffer->Write(queue, source.data());
```

See [MipmapGenerator.cpp](runtime/graphics/src/MipmapGenerator.cpp) for a more complete example.

## Roadmap
- Implement DX12, Metal, and Vulkan backends behind existing interfaces.
- Design a common resource format for runtime loading. Process assets offline in generator module.
- Add basic rendering samples (triangle/texture).
- Scene/Physics/Simulation modules.

#include <slgl/runtime/platform/Platform.hpp>

#include <stdexcept>

#if defined(SLGL_PLATFORM_BACKEND_SDL3)
#include "backends/SDL3/SDL3Backend.hpp"
#endif
#if defined(SLGL_PLATFORM_BACKEND_GLFW)
#include "backends/GLFW/GLFWBackend.hpp"
#endif

namespace SLGL::Platform {
    static Backend* GetSDL3Backend() {
#if defined (SLGL_PLATFORM_BACKEND_SDL3)
        static SDL3::Backend backend;
        return &backend;
#else
        return nullptr;
#endif
    }
    static Backend* GetGLFWBackend() {
#if defined (SLGL_PLATFORM_BACKEND_GLFW)
        static GLFW::Backend backend;
        return &backend;
#else
        return nullptr;
#endif
    }

    Backend* GetBackend(BackendType backend) {
        switch (backend) {
            case BackendType::SDL3:
                return GetSDL3Backend();
            case BackendType::GLFW:
                return GetGLFWBackend();
            default:
                throw std::runtime_error("");
        }
    }
    Backend* GetBackend() {
#if defined(SLGL_PLATFORM_BACKEND_SDL3)
        return GetBackend(BackendType::SDL3);
#elif defined(SLGL_PLATFORM_BACKEND_GLFW)
        return GetBackend(BackendType::GLFW);
#else
#error No available platform backends!
#endif
    }
}
#include "backends/SDL3/SDL3Backend.hpp"

#include "backends/SDL3/SDL3Window.hpp"

#include <SDL3/SDL.h>

#include <stdexcept>
#include <memory>
#include <cstdint>

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace SLGL::Platform {
    SDL3::Backend::Backend() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
            throw std::runtime_error("Failed to initialize SDL3! " + std::string(SDL_GetError()));
        }

        SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "1");

#if defined(_WIN32)
        // Disables Windows scaling
SetProcessDPIAware();
#endif
    }
    SDL3::Backend::~Backend() {
        SDL_Quit();
    }

    bool SDL3::Backend::ShouldQuit() {
        return shouldQuit;
    }

#if defined(_WIN32)
#pragma push_macro("CreateWindow")
#undef CreateWindow
#endif
    Platform::Window::Ref SDL3::Backend::CreateWindow(int width, int height, const std::string& title) {
        return std::make_shared<SDL3::Window>(this, width, height, title);
    }
#if defined(_WIN32)
#pragma pop_macro("CreateWindow")
#endif

    void SDL3::Backend::ProcessEvents() {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    shouldQuit = true;
                    break;
                case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                case SDL_EVENT_WINDOW_RESIZED:
                    windows.at(event.window.windowID)->handleEvent(event);
                    break;
            }
        }
    }

    void SDL3::Backend::registerWindow(uint32_t id, SDL3::Window* window) {
        windows.emplace(id, window);
    }
    void SDL3::Backend::unregisterWindow(uint32_t id) {
        windows.erase(id);
    }
}
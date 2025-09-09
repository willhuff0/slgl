#include "backends/SDL3/SDL3Window.hpp"

#include "backends/SDL3/SDL3Backend.hpp"

#include <SDL3/SDL.h>

#include <string>

namespace SLGL::Platform {
    SDL3::Window::Window(Backend* _backend, int _width, int _height, const std::string &title) {
        backend = _backend;
        width = _width;
        height = _height;

        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetStringProperty(props, SDL_PROP_WINDOW_CREATE_TITLE_STRING, title.c_str());
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, width);
        SDL_SetNumberProperty(props, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, height);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_RESIZABLE_BOOLEAN, true);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_BORDERLESS_BOOLEAN, false);
        SDL_SetBooleanProperty(props, SDL_PROP_WINDOW_CREATE_EXTERNAL_GRAPHICS_CONTEXT_BOOLEAN, true);
        handle = SDL_CreateWindowWithProperties(props);
        if (handle == nullptr) {
            throw std::runtime_error("Failed to create SDL3 window! " + std::string(SDL_GetError()));
        }

        backend->registerWindow(SDL_GetWindowID(handle), this);
    }

    SDL3::Window::~Window() {
        backend->unregisterWindow(SDL_GetWindowID(handle));
    }

    bool SDL3::Window::ShouldClose() {
        return shouldClose;
    }

    int SDL3::Window::GetWidth() const {
        return width;
    }

    int SDL3::Window::GetHeight() const {
        return height;
    }

    void SDL3::Window::SetTitle(const std::string& title) {
        SDL_SetWindowTitle(handle, title.c_str());
    }

    SDL_Window* SDL3::Window::getHandle() const {
        return handle;
    }

    void SDL3::Window::handleEvent(const SDL_Event& event) {
        switch (event.type) {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                shouldClose = true;
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                width = event.window.data1;
                height = event.window.data2;
                notifyWindowSizeChanged(width, height);
                break;
        }
    }
}
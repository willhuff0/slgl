#pragma once

#include <slgl/runtime/platform/Window.hpp>

#include <SDL3/SDL.h>

namespace SLGL::Platform::SDL3 {
    class Backend;

    class Window : public Platform::Window {
    public:
        Window(Backend* backend, int width, int height, const std::string& title);
        ~Window() override;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] bool ShouldClose() override;

        [[nodiscard]] int GetWidth() const override;
        [[nodiscard]] int GetHeight() const override;

        void SetTitle(const std::string& title) override;

        [[nodiscard]] SDL_Window* getHandle() const;

        void handleEvent(const SDL_Event& event);

    private:
        Backend* backend;
        SDL_Window* handle;

        int width;
        int height;
        bool shouldClose = false;
    };
}
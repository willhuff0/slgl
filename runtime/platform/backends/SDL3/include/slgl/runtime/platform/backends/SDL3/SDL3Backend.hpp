#pragma once

#include <slgl/runtime/platform/Backend.hpp>

#include "backends/SDL3/SDL3Window.hpp"

#include <string>
#include <cstdint>
#include <unordered_map>

namespace SLGL::Platform::SDL3 {
    class Backend : public Platform::Backend {
    public:
        Backend();
        ~Backend() override;

        Backend(const Backend&) = delete;
        Backend& operator=(const Backend&) = delete;
        Backend(Backend&&) = delete;
        Backend& operator=(Backend&&) = delete;

        [[nodiscard]] bool ShouldQuit() override;

        [[nodiscard]] Platform::Window::Ref CreateWindow(int width, int height, const std::string& title) override;

        void ProcessEvents() override;

        void registerWindow(SDL_WindowID id, Window* window);
        void unregisterWindow(SDL_WindowID id);

    private:
        bool shouldQuit = false;

        std::unordered_map<SDL_WindowID, Window*> windows;
    };
}
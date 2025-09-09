#pragma once

#include <slgl/runtime/platform/Window.hpp>

#include <memory>

namespace SLGL::Platform {
    class Backend {
    public:
        Backend() = default;
        virtual ~Backend() = default;

        Backend(const Backend&) = delete;
        Backend& operator=(const Backend&) = delete;
        Backend(Backend&&) = delete;
        Backend& operator=(Backend&&) = delete;

        [[nodiscard]] virtual bool ShouldQuit() = 0;

        [[nodiscard]] virtual Window::Ref CreateWindow(int width, int height, const std::string& title) = 0;

        virtual void ProcessEvents() = 0;
    };
}
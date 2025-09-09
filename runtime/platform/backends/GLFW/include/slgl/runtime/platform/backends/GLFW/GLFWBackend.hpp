#pragma once

#include <slgl/runtime/platform/Backend.hpp>

#include <string>

namespace SLGL::Platform::GLFW {
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
    };
}
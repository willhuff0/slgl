#pragma once

#include <slgl/runtime/platform/Window.hpp>

#include <GLFW/glfw3.h>

namespace SLGL::Platform::GLFW {
    class Window : public Platform::Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window() override;

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        [[nodiscard]] bool ShouldClose() override;

        [[nodiscard]] int GetWidth() const override;
        [[nodiscard]] int GetHeight() const override;

        void SetTitle(const std::string& title) override;

        [[nodiscard]] GLFWwindow* getHandle() const;

    private:
        GLFWwindow* handle;

        int width;
        int height;

        void onResize(int width, int height);
    };
}
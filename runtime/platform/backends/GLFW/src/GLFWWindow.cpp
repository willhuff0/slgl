#include "backends/GLFW/GLFWWindow.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

namespace SLGL::Platform {
    GLFW::Window::Window(int _width, int _height, const std::string& title) {
        width = _width;
        height = _height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (handle == nullptr) {
            throw std::runtime_error("Failed to create GLFW window!");
        }

        glfwSetWindowUserPointer(handle, this);
        glfwSetFramebufferSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
            const auto slglWindow = static_cast<GLFW::Window*>(glfwGetWindowUserPointer(window));
            if (slglWindow == nullptr) return;
            slglWindow->onResize(width, height);
        });
    }

    GLFW::Window::~Window() {
        glfwDestroyWindow(handle);
    }

    bool GLFW::Window::ShouldClose() {
        return glfwWindowShouldClose(handle);
    }

    int GLFW::Window::GetWidth() const {
        return width;
    }

    int GLFW::Window::GetHeight() const {
        return height;
    }

    void GLFW::Window::SetTitle(const std::string& title) {
        glfwSetWindowTitle(handle, title.c_str());
    }

    GLFWwindow* GLFW::Window::getHandle() const {
        return handle;
    }

    void GLFW::Window::onResize(int _width, int _height) {
        width = _width;
        height = _height;
        notifyWindowSizeChanged(width, height);
    }
}
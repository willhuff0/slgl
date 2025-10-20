#include "backends/GLFW/GLFWBackend.hpp"

#include "backends/GLFW/GLFWWindow.hpp"

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <memory>

namespace SLGL::Platform {
    GLFW::Backend::Backend() {
        if (glfwInit() != GLFW_TRUE) {
            throw std::runtime_error("Failed to initialize GLFW!");
        }
    }
    GLFW::Backend::~Backend() {
        glfwTerminate();
    }

    bool GLFW::Backend::ShouldQuit() {
        return false;
    }

    Platform::Window::Ref GLFW::Backend::CreateWindow(int width, int height, const std::string& title) {
        return std::make_shared<GLFW::Window>(width, height, title);
    }

    void GLFW::Backend::ProcessEvents() {
        glfwPollEvents();
    }
}
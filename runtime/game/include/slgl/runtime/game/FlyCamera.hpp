#pragma once

#include <glm/glm.hpp>

#include <utility>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class FlyCamera {
public:
    FlyCamera(GLFWwindow* window, glm::vec3 startPos);

    void Tick();

    [[nodiscard]] glm::vec3 GetPos() const;
    [[nodiscard]] glm::mat4 GetProjMat() const;
    [[nodiscard]] glm::mat4 GetViewMat() const;

private:
    GLFWwindow* window;
    double lastTime = 0.0;

    glm::vec3 pos;
    float pitch = 0.0f;
    float yaw = 90.0f;

    float speed = 5.0f;

    glm::mat4 projMat;
    glm::mat4 viewMat;
};

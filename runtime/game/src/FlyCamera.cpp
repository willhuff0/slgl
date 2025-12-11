#include <slgl/runtime/game/FlyCamera.hpp>

#include <algorithm>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define SENSITIVITY 45.0f
#define SCROLL_SENSITIVITY 1.0f

#define FOV glm::radians(80.0f)

static double scrollDelta;
static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    scrollDelta += yOffset;
}

FlyCamera::FlyCamera(GLFWwindow* window, glm::vec3 startPos) : window(window), pos(startPos) {
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    double windowCenterX = (double) windowWidth / 2.0;
    double windowCenterY = (double) windowHeight / 2.0;

    glfwSetCursorPos(window, windowCenterX, windowCenterY);

    lastTime = glfwGetTime();

    glfwSetScrollCallback(window, scrollCallback);
}

void FlyCamera::Tick() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

    double time = glfwGetTime();
    double dt = time - lastTime;
    lastTime = time;

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    float aspectRatio = (float)windowWidth / (float)windowHeight;

    double windowCenterX = (double)windowWidth / 2.0;
    double windowCenterY = (double)windowHeight / 2.0;

    double mousePosX, mousePosY;
    glfwGetCursorPos(window, &mousePosX, &mousePosY);
    float mouseDeltaX = (float)(mousePosX - windowCenterX);
    float mouseDeltaY = (float)(mousePosY - windowCenterY);

    glfwSetCursorPos(window, windowCenterX, windowCenterY);

    const float sensitivity = SENSITIVITY / 800.0f;
    pitch = std::clamp(pitch - mouseDeltaY * sensitivity, -89.9f, 89.9f);
    yaw += mouseDeltaX * sensitivity;

    speed = std::clamp(speed + (float)scrollDelta * SCROLL_SENSITIVITY, 0.1f, 9999999.9f);

    glm::vec3 moveInput(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W)) moveInput.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S)) moveInput.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_D)) moveInput.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A)) moveInput.x -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_UP)) moveInput.z -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_DOWN)) moveInput.z += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT)) moveInput.x += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT)) moveInput.x -= 1.0f;
    if (moveInput != glm::vec3(0.0f)) {
        moveInput = glm::normalize(moveInput);
    }

    if (glfwGetKey(window, GLFW_KEY_X)) yaw = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_Z)) yaw = 90.0f;
    if (glfwGetKey(window, GLFW_KEY_Y)) pitch = 89.9f;

    glm::vec3 forward;
    forward.x = std::cos(glm::radians(pitch)) * std::cos(glm::radians(yaw));
    forward.y = std::sin(glm::radians(pitch));
    forward.z = std::cos(glm::radians(pitch)) * std::sin(glm::radians(yaw));
    if (forward != glm::vec3(0.0f)) {
        forward = glm::normalize(forward);
    }

    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::normalize(glm::cross(right, forward));

    glm::vec3 moveDir = right * moveInput.x + forward * moveInput.z;
    if (glfwGetKey(window, GLFW_KEY_SPACE) || glfwGetKey(window, GLFW_KEY_E)) moveDir.y += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) || glfwGetKey(window, GLFW_KEY_Q)) moveDir.y -= 1.0f;
    if (moveDir != glm::vec3(0.0f)) {
        moveDir = glm::normalize(moveDir);
    }

    const float sprintMultiplier = 2.5f;
    float sprint = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) ? sprintMultiplier : 1.0f;
    pos += moveDir * speed * sprint * (float)dt;

    viewMat = glm::lookAt(pos, pos + forward, glm::vec3(0.0f, 1.0f, 0.0f));

    float nearClip = 0.1f;
    float farClip = 5000.0f;
    projMat = glm::perspective(FOV, aspectRatio, nearClip, farClip);

    scrollDelta = 0.0;
}

glm::vec3 FlyCamera::GetPos() const { return pos; }
glm::mat4 FlyCamera::GetProjMat() const { return projMat; }
glm::mat4 FlyCamera::GetViewMat() const { return viewMat; }

#pragma once

#include "Utils/Utils.h"
#include "Camera.h"

class InputManager
{
public:
    InputManager(Camera* camera);

    void processInput(GLFWwindow* window, float deltaTime);

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
    Camera* camera;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    bool polygonMode = false;
    bool polygonKeyPressed = false;

    int window_width = 800;
    int window_height = 800;

    static InputManager* getInputManager(GLFWwindow* window);
};
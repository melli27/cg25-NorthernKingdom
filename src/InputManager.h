#pragma once

#include "Utils/Utils.h"
#include "Camera.h"
#include <fstream>

class InputManager
{
public:
    InputManager(Camera* camera);

    void processInput(GLFWwindow* window, float deltaTime);

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	static bool getPointLightMode();
	static bool getNormalMappingMode();

private:
    Camera* camera;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    bool polygonMode = false;
    bool polygonKeyPressed = false;

	static bool pointLightMode;
	static bool pointLightKeyPressed;
    static bool normalMappingMode;
	static bool normalMappingKeyPressed;

    bool recordPathMode = false;
	bool recordPathKeyPressed = false;
	float recordInterval = 0.5f; // seconds
	float recordAccumulator = 0.0f;
    std::ofstream recordPathFile;

    int window_width = 800;
    int window_height = 800;

    static InputManager* getInputManager(GLFWwindow* window);

    void toggleCameraRecording();
	void recordCameraPose(float timeSeconds);
};
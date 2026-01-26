#pragma once

#include "Utils/Utils.h"
#include "Camera.h"
#include <fstream>

class Scene;

enum TransformMode {
    TRANSLATE,
    ROTATE
};

class InputManager
{
public:
    InputManager(Camera* camera);

    void setScene(Scene* s) { scene = s; }

    void processInput(GLFWwindow* window, float deltaTime);

    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

	static bool getPointLightMode();

private:
    Camera* camera;

	Scene* scene = nullptr;
    TransformMode currentMode = ROTATE;
	bool tabKeyPressed = false;
    float transformSpeed = 3.0f;
    float pPressed = false;

    bool firstMouse = true;
    float lastX = 0.0f;
    float lastY = 0.0f;

    bool polygonMode = false;
    bool polygonKeyPressed = false;

	static bool pointLightMode;
	static bool pointLightKeyPressed;

    bool recordPathMode = false;
	bool recordPathKeyPressed = false;
	float recordInterval = 0.5f; // seconds
	float recordAccumulator = 0.0f;
    std::ofstream recordPathFile;

	bool fullscreen = false;
    bool fKeyPressed = false;
    //int window_width = 800;
    //int window_height = 800;
	int old_window_width = 800;
	int old_window_height = 800;
    int old_window_x = 100;
    int old_window_y = 100;

    static InputManager* getInputManager(GLFWwindow* window);

    void toggleCameraRecording();
	void recordCameraPose(float timeSeconds);
};
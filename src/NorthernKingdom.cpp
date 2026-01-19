#include "Utils/Utils.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include <Model/Model.h>
#include <InputManager.h>
#include <Scene.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

double camera_fov = 60 * 3.141592 / 180.0;
double camera_near = 0.1;
double camera_far = 1000;

int window_width = 800;
int window_height = 800;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
	// get values from ini file
	INIReader reader("assets/settings.ini");
	window_width = reader.GetInteger("window", "width", 800);
	window_height = reader.GetInteger("window", "height", 800);
	int refresh_rate = reader.GetInteger("window", "refresh_rate", 60);
	std::string window_title = reader.Get("window", "title", "Northern Kingdom");

	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit()) {
		EXIT_WITH_ERROR("Failed to init GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);

#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(reader.GetInteger("window", "width", 800), reader.GetInteger("window", "height", 800), "Northern Kingdom", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//glewExperimental = true;
	if (GLEW_OK != glewInit()) {
		EXIT_WITH_ERROR("Failed to init GLEW");
	}

	Camera camera(camera_fov, (float)window_width / (float)window_height, camera_near, camera_far);
	camera.lastX = window_width / 2.0f;
	camera.lastY = window_height / 2.0f;

	InputManager inputManager(&camera);
	glfwSetWindowUserPointer(window, &inputManager);
	glfwSetCursorPosCallback(window, InputManager::mouseCallback);
	glfwSetScrollCallback(window, InputManager::scrollCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetKeyCallback(window, keyCallback);


	// Create scene objects (Models & Terrain)
	// ------------------------------------
	Scene scene(&camera);
	scene.init();


	// -----------
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		inputManager.processInput(window, deltaTime);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();

		scene.render(window_width, window_height, deltaTime);

		//if (!polygonMode) glEnable(GL_CULL_FACE);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

// TODO: move to InputManager.cpp
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{

	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}
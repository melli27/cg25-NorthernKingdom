// InputManager.cpp
#include "InputManager.h"

bool InputManager::pointLightMode = true;
bool InputManager::pointLightKeyPressed = false;

InputManager::InputManager(Camera* camera) : camera(camera)
{
	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (polygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}
}

void InputManager::processInput(GLFWwindow* window, float deltaTime)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (!camera) return;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->move(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->move(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->move(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->move(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		std::cout << camera->position.x << " " << camera->position.z << std::endl;

	// Space toggle Wireframe
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !polygonKeyPressed)
	{
		polygonKeyPressed = true;
		polygonMode = !polygonMode;

		if (polygonMode)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_CULL_FACE);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		polygonKeyPressed = false;
	}

	// light source toggles
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !pointLightKeyPressed)
	{
		pointLightKeyPressed = true;
		pointLightMode = !pointLightMode;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
	{
		pointLightKeyPressed = false;
	}
}



InputManager* InputManager::getInputManager(GLFWwindow* window)
{
	// Wir nutzen die Window User Pointer, um InputManager zu holen
	return static_cast<InputManager*>(glfwGetWindowUserPointer(window));
}

void InputManager::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	InputManager* input = getInputManager(window);
	if (!input || !input->camera) return;

	float x = static_cast<float>(xpos);
	float y = static_cast<float>(ypos);

	if (input->firstMouse)
	{
		input->lastX = x;
		input->lastY = y;
		input->firstMouse = false;
	}

	float xoffset = x - input->lastX;
	float yoffset = input->lastY - y;

	input->lastX = x;
	input->lastY = y;

	input->camera->rotate(xoffset, yoffset);
}

void InputManager::scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	InputManager* input = getInputManager(window);
	if (!input || !input->camera) return;

	input->camera->zoom(static_cast<float>(yoffset));
}

bool InputManager::getPointLightMode()
{
	return pointLightMode;
}
#include "Utils/Utils.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <vector>
#include "Shader.h"
#include "Geometry.h"
#include "Camera.h"
#include <Model.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

double camera_fov = 60 * 3.141592 / 180.0;
double camera_near = 0.1;
double camera_far = 1000;

bool polygonMode = false;
Shader simpleShader;
Shader normalShader;

// settings
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

	// set refresh rate:
	glfwWindowHint(GLFW_REFRESH_RATE, refresh_rate);

#if _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(reader.GetInteger("window", "width", 800), ("window", "height", 800), "Northern Kingdom", nullptr, nullptr);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//glfwSetKeyCallback(window, keyCallback);

	//glewExperimental = true;
	if (GLEW_OK != glewInit()) {
		EXIT_WITH_ERROR("Failed to init GLEW");
	}

	glClearColor(1, 1, 1, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	if (polygonMode) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);
	}

	//Camera camera(window, camera_fov, (double)800 / (double)800, camera_near, camera_far, false);
	Camera camera(camera_fov, (float)window_width / (float)window_height, camera_near, camera_far);
	camera.lastX = window_width / 2.0f;
	camera.lastY = window_height / 2.0f;


	// build and compile our shader program
	// ------------------------------------

	glfwSetWindowUserPointer(window, &camera);
	//glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Load shader(s)
	simpleShader.createSimpleShader();
	normalShader.createNormalShader();

	// Create geometry
	//Geometry cube = Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(1.5f, 1.5f, 1.5f));
	//Model cube("src/cube.obj");
	//Model backpack("src/backpack/backpack.obj");
	Model castle("src/castle/castle.obj");

	// Transpose model 1
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-2.0f, -1.0f, -3.0f));
	model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
	simpleShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, model);

	// Transpose model 2
	glm::mat4 model2 = glm::mat4(1.0f);
	model2 = glm::translate(model2, glm::vec3(2.0f, 1.0f, -3.0f));
	model2 = glm::scale(model2, glm::vec3(0.3f, 0.3f, 0.3f));
	normalShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, model2);

	// -----------
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = (float)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.getViewMatrix();
		camera.aspectRatio = (float)window_width / (float)window_height;
		glm::mat4 projection = camera.getProjectionMatrix();

		// Draw Backpack with simple shader

		simpleShader.setUniform("viewProjMatrix", projection * view);
		simpleShader.setUniform("modelMatrix", model);
		simpleShader.setUniform("viewPos", camera.position);

		//backpack.Draw(simpleShader);
		castle.Draw(simpleShader);

		//Draw Rotating Castle with normal shader.

		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::rotate(model2, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions

		normalShader.setUniform("viewProjMatrix", projection * view);
		normalShader.setUniform("modelMatrix", modelMat);
		normalShader.setUniform("viewPos", camera.position);
		normalShader.setUniform("lightPos", glm::vec3(2.5f, 1.0f, -1.0f));

		castle.Draw(normalShader);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam->move(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam->move(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam->move(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam->move(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = (float)xposIn;
	float ypos = (float)yposIn;
	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));

	if (cam->firstMouse)
	{
		cam->lastX = xpos;
		cam->lastY = ypos;
		cam->firstMouse = false;
	}

	float xoffset = xpos - cam->lastX;
	float yoffset = cam->lastY - ypos;

	cam->lastX = xpos;
	cam->lastY = ypos;

	cam->rotate(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	cam->zoom(((float)yoffset));
}
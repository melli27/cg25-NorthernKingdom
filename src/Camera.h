#pragma once

#include "Utils/Utils.h"

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

class Camera {
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;
	float fov; // radian

	float aspectRatio;
	float nearPlane;
	float farPlane;

	float movementSpeed = 2.5f;
	float mouseSensitivity = 0.1f;

	float lastX = 0.0f;
	float lastY = 0.0f;
	bool firstMouse = true;

	Camera(float fovStart, float aspectRatioStart, float nearPlaneStart, float farPlaneStart) {
		position = glm::vec3(0.0f, 0.0f, 3.0f);
		worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		front = glm::vec3(0.0f, 0.0f, -1.0f);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
		yaw = -90.0f;
		pitch = 0.0f;
		fov = fovStart;
		aspectRatio = aspectRatioStart;
		nearPlane = nearPlaneStart;
		farPlane = farPlaneStart;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix() const;
	glm::mat4 getProjectionMatrix() const;

	void move(Camera_Movement direction, float deltaTime);
	void rotate(float xoffset, float yoffset, bool constrainPitch = true);
	void zoom(float yoffset);

private:
	void updateCameraVectors();
};
#include "Camera.h"

glm::mat4 Camera::getViewMatrix() const {
	return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
	return glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::move(Camera_Movement direction, float deltaTime) {
	float velocity = movementSpeed * deltaTime;
	if (direction == FORWARD)
		position += front * velocity;
	if (direction == BACKWARD)
		position -= front * velocity;
	if (direction == LEFT)
		position -= right * velocity;
	if (direction == RIGHT)
		position += right * velocity;
}

void Camera::rotate(float xoffset, float yoffset, bool constrainPitch) {
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constrainPitch) {
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	updateCameraVectors();
}

void Camera::zoom(float yoffset) {
	float fovDegrees = glm::degrees(fov);
	fovDegrees -= yoffset;
	if (fovDegrees < 1.0f)
		fovDegrees = 1.0f;
	if (fovDegrees > 90.0f)
		fovDegrees = 90.0f;
	fov = glm::radians(fovDegrees);
}

void Camera::updateCameraVectors() {
	glm::vec3 newFront;
	newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	newFront.y = sin(glm::radians(pitch));
	newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(newFront);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
#include "Camera.h"
#include <fstream>
#include <sstream>

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

	// Terrain collision check
	if (terrain != nullptr) {
		float terrainHeight = terrain->getHeightAt(position.x, position.z);
		if (position.y < terrainHeight + heightOffset) {
			position.y = terrainHeight + heightOffset;
		}
	}
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

void Camera::setPose(const glm::vec3& pos, float yawAngle, float pitchAngle, float fovRad) { 
	position = pos; 
	yaw = yawAngle; 
	pitch = pitchAngle; 
	fov = fovRad; 
	updateCameraVectors();
}

bool Camera::readRecordedCameraPath(const std::string& filePath) {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		std::cerr << "Failed to open camera path file: " << filePath << std::endl;
		return false;
	}

	recordedPoses.clear();
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream iss(line);
		CameraPose pose;
		if (!(iss >> pose.time >> pose.position.x >> pose.position.y >> pose.position.z
			>> pose.yaw >> pose.pitch >> pose.fov)) {
			std::cerr << "Error reading camera pose from line: " << line << std::endl;
			continue;
		}
		recordedPoses.push_back(pose);
	}
	file.close();
	return true;
}

void Camera::startPlayback(float timeSeconds) {
	if (recordedPoses.empty()) {
		std::cerr << "No recorded camera poses available for playback." << std::endl;
		return;
	}
	playbackMode = true;
	playbackStartTime = timeSeconds;
	pathStartTime = recordedPoses.front().time;
	currentPoseIndex = 0;
}

void Camera::stopPlayback() {
	playbackMode = false;
}

bool Camera::updatePlayback(float timeSeconds) {
	if (!playbackMode || recordedPoses.empty())
		return false;

	float elapsedTime = timeSeconds - playbackStartTime + pathStartTime;

	if (elapsedTime >= recordedPoses.back().time) {
		const auto& lastPose = recordedPoses.back();
		setPose(lastPose.position, lastPose.yaw, lastPose.pitch, glm::radians(lastPose.fov));
		playbackMode = false;
		return false;
	}

	while (currentPoseIndex < recordedPoses.size() - 1 &&
		elapsedTime >= recordedPoses[currentPoseIndex + 1].time) {
		currentPoseIndex++;
	}

	const CameraPose& poseA = recordedPoses[currentPoseIndex];
	const CameraPose& poseB = recordedPoses[currentPoseIndex + 1];
	float segmentDuration = poseB.time - poseA.time;
	float factor = (elapsedTime - poseA.time) / segmentDuration;
	
	setPose(
		glm::mix(poseA.position, poseB.position, factor),
		glm::mix(poseA.yaw, poseB.yaw, factor),
		glm::mix(poseA.pitch, poseB.pitch, factor),
		glm::radians(glm::mix(poseA.fov, poseB.fov, factor))
	);

	return true;
}
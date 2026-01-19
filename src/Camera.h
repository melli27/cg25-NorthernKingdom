#pragma once

#include "Utils/Utils.h"
#include <Terrain/Terrain.h>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

struct CameraPose {
	glm::vec3 position;
	float yaw;
	float pitch;
	float fov;
	float time;
};

class Camera {
public:
	Terrain* terrain = nullptr;
	float heightOffset = 2.0f; // Mindestabstand zum Terrain
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

	float movementSpeed = 8.5f;
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
		yaw = 120.0f;
		pitch = -15.0f;
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

	void setPose(const glm::vec3& pos, float yawAngle, float pitchAngle, float fovRad);
	bool readRecordedCameraPath(const std::string& filePath);
	void startPlayback(float timeSeconds);
	void stopPlayback();
	bool getPlaybackMode() const { return playbackMode; }
	bool updatePlayback(float timeSeconds);

	void setTerrain(Terrain* t) { terrain = t; }
	void setHeightOffset(float offset) { heightOffset = offset; }

private:
	void updateCameraVectors();

	std::vector<CameraPose> recordedPoses;
	bool playbackMode = false;
	float playbackStartTime = 0.0f;
	float pathStartTime = 0.0f;
	size_t currentPoseIndex = 0;
};
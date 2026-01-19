#include "LightManager.h"

LightManager::LightManager() {
}

LightManager::~LightManager() {
}

void LightManager::addDirectionalLight(const DirectionalLight& light) {
    directionalLights.push_back(light);
}

void LightManager::addPointLight(const PointLight& light) {
    pointLights.push_back(light);
}

void LightManager::applyToShader(Shader& shader) {
    shader.activate();

    // Apply directional lights (assuming single light for simplicity)
    if (!directionalLights.empty()) {
        directionalLights[0].applyToShader(shader, "dirLight");
    }

    // Apply point lights (assuming single light for simplicity)
    if (!pointLights.empty()) {
        pointLights[0].applyToShader(shader, "pointLight");
    }
}

mat4 LightManager::calculateLightSpaceMatrix() const
{
	DirectionalLight dirLight = directionalLights[0]; // TODO change if multiple lights

	// Light space transformation matrix
	float nearPlane = 1.0f, farPlane = 90.0f; 
	float orthoSize = 20.0f; // 4.0f
	vec3 sceneCenter = glm::vec3(0.0f,-84.3137f, 25.0f);
	vec3 lightPosition = sceneCenter - normalize(dirLight.direction) * 30.0f;
	mat4 lightProjection = ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
	mat4 lightView = lookAt(lightPosition, sceneCenter, vec3(0.0, 1.0, 0.0));
	mat4 lightSpaceMatrix = lightProjection * lightView;

	return lightSpaceMatrix;
}

std::vector<mat4> LightManager::calculateShadowTransforms() const
{
	PointLight pointLight = pointLights[0]; // TODO change if multiple lights

	// Light space depth cubemap transformation matrices
	float pointNearPlane = 1.0f, pointFarPlane = 25.0f;
	glm::vec3 pointLightPos = pointLight.position;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, pointNearPlane, pointFarPlane);
	std::vector<glm::mat4> shadowTransforms;
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	
	return shadowTransforms;
}

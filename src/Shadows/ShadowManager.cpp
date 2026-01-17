#include "Shadows/ShadowManager.h"

ShadowManager::ShadowManager() {}

ShadowManager::~ShadowManager() {}

void ShadowManager::init() {
	depthShader.createDepthShader();
	pointDepthShader.createPointDepthShader();

	// Directional depth map FBO
	glGenFramebuffers(1, &dirDepthFBO);
	// depth texture
	glGenTextures(1, &dirDepthMap);
	glBindTexture(GL_TEXTURE_2D, dirDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, dirShadowWidth, dirShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, dirDepthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dirDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Point depth map FBO
	glGenFramebuffers(1, &pointDepthFBO);
	// depth cubemap texture
	glGenTextures(1, &pointDepthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, pointShadowWidth, pointShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pointDepthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowManager::updateDirectionalLightSpaceTransform(const DirectionalLight& dirLight) {
	float nearPlane = 1.0f;
	float farPlane = 40.0f;
	float orthoSize = 10.0f; // 4.0f

	glm::vec3 sceneCenter = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightPosition = sceneCenter - glm::normalize(dirLight.direction) * 20.0f;
	glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
	glm::mat4 lightView = glm::lookAt(lightPosition, sceneCenter, glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

void ShadowManager::updatePointLightSpaceTransforms(const PointLight& pointLight) {
	float nearPlane = 1.0f;
	float farPlane = pointFarPlane;
	shadowTransforms.clear();

	glm::vec3 pointLightPos = pointLight.position;
	glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
	shadowTransforms.push_back(shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
}

void ShadowManager::beginDirectionalShadowPass() {
	glViewport(0, 0, dirShadowWidth, dirShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, dirDepthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void ShadowManager::endDirectionalShadowPass() {
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowManager::beginPointShadowPass() {
	glViewport(0, 0, pointShadowWidth, pointShadowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, pointDepthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void ShadowManager::endPointShadowPass() {
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowManager::applyToLightingShader(Shader& lightingShader, int window_width, int window_height) const {
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const int dirShadowUnit = 3;
	glActiveTexture(GL_TEXTURE0 + dirShadowUnit);
	glBindTexture(GL_TEXTURE_2D, dirDepthMap);

	const int pointShadowUnit = 4;
	glActiveTexture(GL_TEXTURE0 + pointShadowUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, pointDepthMap);

	lightingShader.activate();
	lightingShader.setUniform("shadowMap", dirShadowUnit);
	lightingShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);
	lightingShader.setUniform("pointShadowMap", pointShadowUnit);
	lightingShader.setUniform("farPlane", pointFarPlane);
}

Shader& ShadowManager::getDepthShader() {
	return depthShader;
}

Shader& ShadowManager::getPointDepthShader() {
	return pointDepthShader;
}

const glm::mat4& ShadowManager::getLightSpaceMatrix() const {
	return lightSpaceMatrix;
}

const std::vector<glm::mat4>& ShadowManager::getPointShadowTransforms() const {
	return shadowTransforms;
}

float ShadowManager::getPointFarPlane() const {
	return pointFarPlane;
}
#pragma once
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"

class ShadowManager {
public:
	ShadowManager();
	~ShadowManager();

	void init();

	void updateDirectionalLightSpaceTransform(const DirectionalLight& dirLight);
	void updatePointLightSpaceTransforms(const PointLight& pointLight);

	void beginDirectionalShadowPass();
	void endDirectionalShadowPass();
	void beginPointShadowPass();
	void endPointShadowPass();
	void applyToLightingShader(Shader& lightingShader, int window_width, int window_height) const;
	
	Shader& getDepthShader();
	Shader& getPointDepthShader();
	const glm::mat4& getLightSpaceMatrix() const;
	const std::vector<glm::mat4>& getPointShadowTransforms() const;
	float getPointFarPlane() const;

private:
	// Directional light shadow
	GLuint dirDepthFBO = 0;
	GLuint dirDepthMap = 0;
	unsigned int dirShadowWidth = 2048;
	unsigned int dirShadowHeight = 2048;
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);

	// Point light shadow
	GLuint pointDepthFBO = 0;
	GLuint pointDepthMap = 0;
	unsigned int pointShadowWidth = 1024;
	unsigned int pointShadowHeight = 1024;
	std::vector<glm::mat4> shadowTransforms;

	Shader depthShader;
	Shader pointDepthShader;

	float pointFarPlane = 25.0f;
};
#pragma once
#include "Light.h"
#include "Shader.h"

class PointLight : public Light {
public:
	glm::vec3 position;
	glm::vec3 attenuation; // (constant, linear, quadratic)
	PointLight() = default;
	PointLight(const glm::vec3& position, const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& attenuation)
		: Light(color, ambient, diffuse, specular), position(position), attenuation(attenuation) {
	}

	void applyToShader(Shader& shader, const std::string& uniformName) const;
};
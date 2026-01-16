#pragma once
#include "Light.h"
#include "Shader.h"

class DirectionalLight : public Light {
public:
	glm::vec3 direction;
	DirectionalLight() = default;
	DirectionalLight(const glm::vec3& direction, const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
		: Light(color, ambient, diffuse, specular), direction(direction) {
	}

	void applyToShader(Shader& shader, const std::string& uniformName) const;
};
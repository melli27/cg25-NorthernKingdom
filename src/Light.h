#pragma once

#include "Utils/Utils.h"

class Light {
public:
	glm::vec3 color;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	Light() = default;
	Light(const glm::vec3& color, const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
		: color(color), ambient(ambient), diffuse(diffuse), specular(specular) {
	}
};
#include "PointLight.h"

void PointLight::applyToShader(Shader& shader, const std::string& uniformName) const {
	shader.setUniform(uniformName + ".position", position);
	shader.setUniform(uniformName + ".color", color);
	shader.setUniform(uniformName + ".ambient", ambient);
	shader.setUniform(uniformName + ".diffuse", diffuse);
	shader.setUniform(uniformName + ".specular", specular);
	shader.setUniform(uniformName + ".attenuation", attenuation);
}
#include "DirectionalLight.h"

void DirectionalLight::applyToShader(Shader& shader, const std::string& uniformName) const {
	shader.setUniform(uniformName + ".direction", direction);
	shader.setUniform(uniformName + ".color", color);
	shader.setUniform(uniformName + ".ambient", ambient);
	shader.setUniform(uniformName + ".diffuse", diffuse);
	shader.setUniform(uniformName + ".specular", specular);
}
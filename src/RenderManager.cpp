#include "RenderManager.h"
#include <GL/glew.h>

RenderManager::RenderManager() {
}

RenderManager::~RenderManager() {
}

void RenderManager::renderModel(Model* model, Shader& shader, const glm::mat4& modelMatrix,
	Camera* camera, const glm::mat4& viewProj) {
	if (!model) {
		return;
	}

	shader.activate();
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", modelMatrix);
	shader.setUniform("viewPos", camera->position);
	shader.setUniform("isAnimated", false);

	model->draw(shader);
}

void RenderManager::renderAnimatedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, Camera* camera, const glm::mat4& viewProj) {
	shader.activate();

	// Set standard uniforms
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", modelMatrix);
	shader.setUniform("viewPos", camera->position);
	shader.setUniform("isAnimated", true);

	// Get and send bone matrices
	vector<glm::mat4> transformationMatrices = model->getBoneTransforms(glfwGetTime(), mat4(1.0f));
	for (unsigned int i = 0; i < transformationMatrices.size(); i++) {
		glm::mat4 mat = transformationMatrices[i];
		shader.setUniformMatrix4fv("boneMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, mat);
	}

	//model->animate(shader, modelMatrix, glfwGetTime(), glm::vec3(0.0f), 0.0f, 0.0f);

	// Draw the model
	model->draw(shader);
}


void RenderManager::renderLightCube(Geometry* lightCube, Shader& shader, const glm::mat4& viewProj) {
	if (!lightCube) {
		return;
	}

	shader.activate();
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", lightCube->getModelMatrix());
	shader.setUniform("normalMatrix", lightCube->getNormalMatrix());

	lightCube->draw();
}

void RenderManager::renderTerrain(Terrain* terrain, Shader& shader, const glm::mat4& modelMatrix,
	const glm::mat4& view, const glm::mat4& projection,
	const TerrainRenderParams& params) {
	if (!terrain) {
		return;
	}

	shader.activate();

	// Set transform matrices
	shader.setUniform("model", modelMatrix);
	shader.setUniform("view", view);
	shader.setUniform("projection", projection);

	// Set terrain-specific parameters
	shader.setUniform("cameraPos", params.cameraPos);
	shader.setUniform("minDistance", params.minDistance);
	shader.setUniform("maxDistance", params.maxDistance);
	shader.setUniform("minTessLevel", params.minTessLevel);
	shader.setUniform("maxTessLevel", params.maxTessLevel);

	// Render with special settings
	glDisable(GL_CULL_FACE);
	terrain->Draw(shader);
	glEnable(GL_CULL_FACE);
}
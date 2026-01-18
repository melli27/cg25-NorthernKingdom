#include "RenderManager.h"
#include <GL/glew.h>

RenderManager::RenderManager() {
}

RenderManager::~RenderManager() {
}


//void RenderManager::renderDepthmap(Depthmap* depthmap, Shader& shader, mat4 lightSpaceMatrix, map<Model*, mat4> modelMatrixMap)
//{
//	depthmap->DephtmapRenderSetup();
//	shader.activate();
//	shader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);
//
//	for (const auto& [model, modelMatrix] : modelMatrixMap) {
//		shader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, modelMatrix);
//		model->draw(shader);
//	}
//}

void RenderManager::renderModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPosition, const mat4& viewProj)
{
	if (!model) {
		return;
	}

	shader.activate();
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", modelMatrix);
	shader.setUniform("viewPos", cameraPosition);
	shader.setUniform("isAnimated", false);

	model->draw(shader);
}

void RenderManager::renderShadedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPosition, const glm::mat4& viewProj, const mat4& lightspaceMatrix, bool isAnimated)
{
	if (!model) {
		return;
	}

	shader.activate();
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", modelMatrix);
	shader.setUniform("viewPos", cameraPosition);
	shader.setUniform("isAnimated", isAnimated);

	shader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightspaceMatrix);
	shader.setUniform("shadowMap", 3);
	shader.setUniform("pointShadowMap", 4);
	shader.setUniform("farPlane", 25.0f);//TODO get from lightmanager pointfarplane

	model->draw(shader);
	
}

void RenderManager::setAnimated(Shader& shader, vector<mat4>& transformationMatrices)
{
	shader.activate();

	shader.setUniform("isAnimated", true);

	// Get and send bone matrices
	for (unsigned int i = 0; i < transformationMatrices.size(); i++) {
		glm::mat4 mat = transformationMatrices[i];
		shader.setUniformMatrix4fv("boneMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, mat);
	}
}

void RenderManager::renderAnimatedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPosition, const glm::mat4& viewProj, vector<mat4>& transformationMatrices) {
	shader.activate();

	// Set standard uniforms
	shader.setUniform("viewProjMatrix", viewProj);
	shader.setUniform("modelMatrix", modelMatrix);
	shader.setUniform("viewPos", cameraPosition);
	shader.setUniform("isAnimated", true);

	// Get and send bone matrices
	for (unsigned int i = 0; i < transformationMatrices.size(); i++) {
		glm::mat4 mat = transformationMatrices[i];
		shader.setUniformMatrix4fv("boneMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, mat);
	}
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

void RenderManager::renderTerrain(Terrain* terrain, Shader& shader, const glm::mat4& modelMatrix, const glm::mat4& view, const glm::mat4& projection, unsigned int depthMap, glm::mat4 lightSpaceMatrix,
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
	terrain->Draw(shader, depthMap, lightSpaceMatrix);
	glEnable(GL_CULL_FACE);
}
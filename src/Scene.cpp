#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <RenderManager.h>
#include <Lights/LightManager.h>

Scene::Scene(Camera* cam)
	: camera(cam),
	renderManager(new RenderManager()),
	lightManager(new LightManager()),
	terrain(nullptr),
	backpack(nullptr),
	lightCube(nullptr)
{
}

Scene::~Scene() {
	delete renderManager;
	delete lightManager;
	delete terrain;
	delete backpack;
	delete lightCube;
}

void Scene::init() {

	// Lighting shader
	lightSourceShader.createLightSourceShader();
	lightingShader.createLightingShader();

	// Depth shaders
	depthShader.createDepthShader();
	pointDepthShader.createPointDepthShader();

	// Terrain and animated model shaders
	terrainShader.createTerrainShader();
	animatedModelShader.createAnimatedModelShader();

	// Setup Lights
	lightManager->addDirectionalLight(dirLight);
	lightManager->addPointLight(pointLight);
	lightManager->applyToShader(lightingShader);
	lightSpaceMatrix = lightManager->calculateLightSpaceMatrix();
	shadowTransforms = lightManager->calculateShadowTransforms();

	// Load Terrain / Models
	lightCube = new Geometry(glm::translate(glm::mat4(1.0f), pointLight.position), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f));
	testCube = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 2.0f)), Geometry::createCubeGeometry(1.0f, 1.0f, 1.0f));
	testCube->transform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.45f, 0.0f)));
	//lightCube = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f));

	terrain = new Terrain(terrainShader, "assets/heightmap.png");
	backpack = new Model("assets/models/backpack/backpack.obj", false);
	//castleGuard = new Model("assets/models/Reaction/reaction.dae", true);
	castleGuard = new Model("assets/models/castle_guard/castle_guard.dae", true);
	
	// Load Animations
	castleGuardAnimation = new Animation("assets/models/Reaction/Reaction.dae", castleGuard);
	animator = new Animator(castleGuardAnimation);

	// Setup Model Transforms
	backpackModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, -3.0f));
	backpackModelMatrix = glm::scale(backpackModelMatrix, glm::vec3(0.3f));
	castleGuardModelMatrix = glm::mat4(1.0f); //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//castleGuardModelMatrix = glm::scale(castleGuardModelMatrix, glm::vec3(0.5f));
	terrainModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 36.0f, 0.0f)), glm::vec3(0.5f));

	// Setup Depthmap
	depthmap = new Depthmap();
	depthmap->initDepthmap();

}

void Scene::render(int window_width, int window_height, float deltaTime)
{
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// 1. pass: render to depth map
	// ----------------------------
	depthmap->renderToDepthmap();
	depthShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);

	// Backpack depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	backpack->draw(depthShader);

	// Testcube draw
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, testCube->getModelMatrix());
	testCube->draw();

	// 1. pass: render to depth cubemap
	// --------------------------------
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	depthmap->renderToDepthCubemap();

	pointDepthShader.setUniform("lightPos", pointLight.position);
	pointDepthShader.setUniform("farPlane", 25.0f); //TODO get from lightmanager pointfarplane
	for (unsigned int i = 0; i < 6; ++i)
		pointDepthShader.setUniformMatrix4fv("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, shadowTransforms[i]);

	// Backpack depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	backpack->draw(pointDepthShader);

	// Test cube draw
	pointDepthShader.setUniform("modelMatrix", testCube->getModelMatrix());
	testCube->draw();

	//glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 2. pass: render scene normally with shadow mapping
	// --------------------------------------------------
	depthmap->normalRenderSetup(window_width, window_height);

	// Render castle guard with animation
	animator->UpdateAnimation(deltaTime);
	auto transforms = animator->GetFinalBoneMatrices();
	renderManager->renderAnimatedModel(castleGuard, animatedModelShader, castleGuardModelMatrix, camera->position, viewProj, transforms);

	// Render backpack with lighting
	renderManager->renderShadedModel(backpack, lightingShader, backpackModelMatrix, camera->position, viewProj, lightSpaceMatrix);

	// Render light cube
	renderManager->renderLightCube(lightCube, lightSourceShader, viewProj);

	// Render terrain
	TerrainRenderParams terrainParams;
	terrainParams.cameraPos = camera->position;
	terrainParams.minDistance = 2.0f;
	terrainParams.maxDistance = 30.0f;
	terrainParams.minTessLevel = 2.0f;
	terrainParams.maxTessLevel = 16.0f;
	renderManager->renderTerrain(terrain, terrainShader, terrainModelMatrix, view, projection, terrainParams);

	glDisable(GL_CULL_FACE);
	terrain->Draw(terrainShader);
}
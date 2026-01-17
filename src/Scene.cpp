#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <RenderManager.h>
#include <Lights/LightManager.h>

Scene::Scene(Camera* cam)
	: camera(cam),
	renderManager(new RenderManager()),
	lightManager(new LightManager()),
	shadowManager(new ShadowManager()),
	terrain(nullptr),
	backpack(nullptr),
	lightCube(nullptr)
{
}

Scene::~Scene() {
	delete renderManager;
	delete lightManager;
	delete shadowManager;
	delete terrain;
	delete backpack;
	delete lightCube;
}

void Scene::init() {

	// Create shaders
	lightSourceShader.createLightSourceShader();
	lightingShader.createLightingShader();
	terrainShader.createTerrainShader();

	dirLight = DirectionalLight(
		glm::vec3(-0.4f, -0.6f, -0.2f),
		glm::vec3(1.0f, 0.9f, 0.7f),
		glm::vec3(0.2f),
		glm::vec3(0.9f, 0.6f, 0.4f),
		glm::vec3(0.9f, 0.9f, 0.8f)
	);

	pointLight = PointLight(
		glm::vec3(-4.0f, -1.5f, -3.0f),
		glm::vec3(1.0f),
		glm::vec3(0.2f),
		glm::vec3(0.5f),
		glm::vec3(1.0f),
		glm::vec3(1.0f, 0.09f, 0.032f)
	);

	lightManager->addDirectionalLight(dirLight);
	lightManager->addPointLight(pointLight);
	lightManager->applyToShader(lightingShader);

	shadowManager->init();

	// Load Terrain / Models
	lightCube = new Geometry(glm::translate(glm::mat4(1.0f), pointLight.position), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f));
	terrain = new Terrain(terrainShader, "assets/heightmap.png");
	backpack = new Model("assets/models/backpack/backpack.obj");
	castleGuard = new Model("assets/models/castle_guard/castle_guard.dae");

	// Load Animations
	//castleGuardAnimation = new Animation(castleGuard, 0);
	//castleGuardPlayer = new AnimationPlayer(castleGuardAnimation);
	//castleGuardPlayer->setLoop(true);
	//castleGuardPlayer->setSpeed(0.8f);
	//castleGuardPlayer->play();

	// Setup Model Transforms
	backpackModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, -3.0f));
	backpackModelMatrix = glm::scale(backpackModelMatrix, glm::vec3(0.3f));

	castleGuardModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -4.0f));
	castleGuardModelMatrix = glm::scale(castleGuardModelMatrix, glm::vec3(0.3f));
	
	terrainModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 36.0f, 0.0f)), glm::vec3(0.5f));

	lightCubeModelMatrix = glm::translate(glm::mat4(1.0f), pointLight.position);
}

void Scene::render(float deltaTime) {

	//castleGuardPlayer->update(deltaTime);

	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// Update light space matrices for shadows
	shadowManager->updateDirectionalLightSpaceTransform(dirLight);
	shadowManager->updatePointLightSpaceTransforms(pointLight);

	// 1. pass: render to depth map
	shadowManager->beginDirectionalShadowPass();
	{
		Shader& dirDepthShader = shadowManager->getDepthShader();
		dirDepthShader.activate();
		dirDepthShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, shadowManager->getLightSpaceMatrix());
		dirDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
		backpack->draw(dirDepthShader);
	}
	shadowManager->endDirectionalShadowPass();

	// 1.pass: render to depth cubemap
	shadowManager->beginPointShadowPass();
	{
		Shader& pointDepthShader = shadowManager->getPointDepthShader();
		pointDepthShader.activate();
		pointDepthShader.setUniform("farPlane", shadowManager->getPointFarPlane());
		pointDepthShader.setUniform("lightPos", pointLight.position);
		const auto& shadowTransforms = shadowManager->getPointShadowTransforms();
		for (unsigned int i = 0; i < 6; i++) {
			pointDepthShader.setUniformMatrix4fv("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, shadowTransforms[i]);
		}
		pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
		backpack->draw(pointDepthShader);
	}
	shadowManager->endPointShadowPass();

	// 2. pass: render scene normally with shadow mapping
	shadowManager->applyToLightingShader(lightingShader, 800, 800);

	// Render terrain
	TerrainRenderParams terrainParams;
	terrainParams.cameraPos = camera->position;
	terrainParams.minDistance = 2.0f;
	terrainParams.maxDistance = 30.0f;
	terrainParams.minTessLevel = 2.0f;
	terrainParams.maxTessLevel = 16.0f;

	renderManager->renderTerrain(terrain, terrainShader, terrainModelMatrix, view, projection, terrainParams);

	// Render castle guard with animation
	//renderManager->renderAnimatedModel(castleGuard, lightingShader, castleGuardModelMatrix, camera, viewProj);

	// Render backpack with lighting
	renderManager->renderModel(backpack, lightingShader, backpackModelMatrix, camera, viewProj);

	// Render light cube
	//renderManager->renderLightCube(lightCube,lightSourceShader, lightCubeModelMatrix, viewProj);

	glDisable(GL_CULL_FACE);
	terrain->Draw(terrainShader);
}
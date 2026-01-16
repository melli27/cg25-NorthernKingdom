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

	// Create shaders
	lightSourceShader.createLightSourceShader();
	lightingShader.createLightingShader();
	terrainShader.createTerrainShader();

	DirectionalLight dirLight = DirectionalLight(
		glm::vec3(-0.4f, -0.6f, -0.2f),
		glm::vec3(1.0f, 0.9f, 0.7f),
		glm::vec3(0.2f),
		glm::vec3(0.9f, 0.6f, 0.4f),
		glm::vec3(0.9f, 0.9f, 0.8f)
	);

	PointLight pointLight = PointLight(
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

}

void Scene::render(float deltaTime) {

	//castleGuardPlayer->update(deltaTime);

	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// Render terrain
	TerrainRenderParams terrainParams;
	terrainParams.cameraPos = camera->position;
	terrainParams.minDistance = 2.0f;
	terrainParams.maxDistance = 30.0f;
	terrainParams.minTessLevel = 2.0f;
	terrainParams.maxTessLevel = 16.0f;

	renderManager->renderTerrain(terrain, terrainShader, terrainModelMatrix, view, projection, terrainParams);

	// Render castle guard with animation
	renderManager->renderAnimatedModel(castleGuard, lightingShader, castleGuardModelMatrix, camera, viewProj);

	// Render backpack with lighting
	renderManager->renderModel(backpack, lightingShader, backpackModelMatrix, camera, viewProj);

	// Render light cube
	renderManager->renderLightCube(lightCube,lightSourceShader, viewProj);

	glDisable(GL_CULL_FACE);
	terrain->Draw(terrainShader);
}
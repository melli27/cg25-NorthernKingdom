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
	lightCube(nullptr),
	skybox(nullptr)
{
}

Scene::~Scene() {
	delete renderManager;
	delete lightManager;
	delete terrain;
	delete backpack;
	delete lightCube;
	delete skybox;
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

	// Skybox shader
	skyboxShader.createSkyboxShader();

	// Setup Lights
	lightManager->addDirectionalLight(dirLight);
	lightManager->addPointLight(pointLight);
	lightManager->applyToShader(lightingShader);
	lightSpaceMatrix = lightManager->calculateLightSpaceMatrix();
	shadowTransforms = lightManager->calculateShadowTransforms();

	// Load Terrain / Models
	lightCube = new Geometry(glm::translate(glm::mat4(1.0f), pointLight.position), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f));
	//lightCube = new Geometry(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), Geometry::createCubeGeometry(0.2f, 0.2f, 0.2f));

	terrain = new Terrain(terrainShader, "assets/heightmap.png");
	backpack = new Model("assets/models/backpack/backpack.obj", false);
	house = new Model("assets/models/city_house_2/city_house_2_bi.dae", true);
	castleGuard = new Model("assets/models/castle_guard/castle_guard.dae", true);

	// Load Animations
	castleGuardAnimation = new Animation("assets/models/Reaction/Reaction.dae", castleGuard);
	animator = new Animator(castleGuardAnimation);

	// Setup Model Transforms
	backpackModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, -3.0f));
	backpackModelMatrix = glm::scale(backpackModelMatrix, glm::vec3(0.3f));
	houseMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -2.5f, -4.0f));
	houseMatrix = glm::rotate(houseMatrix, glm::radians(180.0f), vec3(0.0, 1.0, 1.0) );
	castleGuardModelMatrix = glm::mat4(1.0f); //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	//castleGuardModelMatrix = glm::scale(castleGuardModelMatrix, glm::vec3(0.5f));
	terrainModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 36.0f, 0.0f)), glm::vec3(0.5f));

	// Setup Depthmap
	depthmap = new Depthmap();
	depthmap->initDepthmap();

	// Setup Skybox
	skybox = new Skybox();
	skybox->init("assets/textures/sky");
}

void Scene::render(int window_width, int window_height, float deltaTime)
{
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// Update animation
	animator->UpdateAnimation(deltaTime);
	auto boneMatrices = animator->GetFinalBoneMatrices();

	// 1. pass: render to depth map
	// ----------------------------
	depthmap->DephtmapRenderSetup();
	depthShader.activate();
	depthShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);

	// Backpack depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	depthShader.setUniform("isAnimated", false);
	backpack->draw(depthShader);

	// House depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix);
	depthShader.setUniform("isAnimated", false);
	house->draw(depthShader);

	// Guard depth
	renderManager->setAnimated(depthShader, boneMatrices);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, castleGuardModelMatrix);
	castleGuard->draw(depthShader);

	// 1. pass: render to depth cubemap
	// --------------------------------
	depthmap->CubemapRenderSetup();

	pointDepthShader.activate();
	pointDepthShader.setUniform("lightPos", pointLight.position);
	pointDepthShader.setUniform("farPlane", 25.0f); //TODO get from lightmanager pointfarplane
	for (unsigned int i = 0; i < 6; ++i)
		pointDepthShader.setUniformMatrix4fv("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, shadowTransforms[i]);

	// Backpack depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	backpack->draw(pointDepthShader);

	// House depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix);
	house->draw(pointDepthShader);

	// Guard depth
	renderManager->setAnimated(depthShader, boneMatrices);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, castleGuardModelMatrix);
	castleGuard->draw(pointDepthShader);

	// 2. pass: render scene normally with shadow mapping
	// --------------------------------------------------
	depthmap->normalRenderSetup(window_width, window_height);
	
	// Render Models with lighting
	renderManager->renderShadedModel(backpack, lightingShader, backpackModelMatrix, camera->position, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(house, lightingShader, houseMatrix, camera->position, viewProj, lightSpaceMatrix, false);

	renderManager->setAnimated(lightingShader, boneMatrices);
	renderManager->renderShadedModel(castleGuard, lightingShader, castleGuardModelMatrix, camera->position, viewProj, lightSpaceMatrix, true);

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

	// Render skybox
	skybox->draw(skyboxShader, view, projection);
}
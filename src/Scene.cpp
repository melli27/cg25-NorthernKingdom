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
	skybox(nullptr),
	streetLight(nullptr)
{
}

Scene::~Scene() {
	delete renderManager;
	delete lightManager;
	delete terrain;
	delete backpack;
	delete lightCube;
	delete skybox;
	delete streetLight;
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

	// Skybox shader
	skyboxShader.createSkyboxShader();

	// Load Terrain / Models
	terrain = new Terrain(terrainShader, "assets/heightmap2.png");
	camera->setTerrain(terrain);
	camera->setHeightOffset(2.0f);
	backpack = new Model("assets/models/backpack/backpack.obj", false);
	house = new Model("assets/models/city_house_2/city_house_2_bi.dae", true);
	tower = new Model("assets/models/Medieval tower/Medieval tower_High/Medieval tower_mid.dae", true);
	castleGuard = new Model("assets/models/castle_guard/castle_guard.dae", true);
	girl = new Model("assets/models/Peasant Girl/Peasant Girl.dae", true);
	pavement = new Model("assets/models/pavement/pavement.obj", true);
	streetLight = new Model("assets/models/Street Light/street_light.obj", true);
	
	// Set textures
	tower->setTexture("assets/models/Medieval tower/Medieval tower_mid_Col.jpg", "assets/models/Medieval tower/Medieval tower_mid_spec.jpg", "assets/models/Medieval tower/Medieval tower_mid_Nor.jpg");

	//lamp = new Model("assets/models/lamp/lamp1.obj", true);
	//lamp->setTexture("assets/models/lamp/lamp1.png", nullptr, "assets/models/lamp/lamp1normal.jpg");
	bigHouse = new Model("assets/models/small_building_1/small_building_1.dae", true);
	
	// Load Animations
	castleGuardAnimation = new Animation("assets/models/Unarmed Idle Looking Ver. 2.dae", castleGuard);
	animator = new Animator(castleGuardAnimation);
	catwalk = new Animation("assets/models/Catwalk Walk Turn 180 Tight.dae", girl);
	animator2 = new Animator(catwalk);

	// Get Terrain height
	terrainModelMatrix = glm::mat4(1.0f);
	float terrainHeight = terrain->getHeightAt(-3.0f, 20.0f);
	cout << "Terrain height at (0,0): " << terrainHeight << endl;

	camera->position = glm::vec3(15.0f, terrainHeight + 10.0f, -2.0f);

	// Setup Model Transforms
	backpackModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(8.0f, terrainHeight + 4.0, 22.0f));
	backpackModelMatrix = glm::scale(backpackModelMatrix, glm::vec3(0.3f));

	houseMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(11.0f, terrainHeight, 25.0f)), glm::vec3(1.5));
	houseMatrix = glm::rotate(houseMatrix, glm::radians(180.0f), vec3(0.0, 1.0, 1.0));
	houseMatrix = glm::rotate(houseMatrix, glm::radians(45.0f), vec3(0.0, 0.0, 1.0));

	houseMatrix2 = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5.0, terrainHeight, 30.0)), glm::vec3(1.5));
	houseMatrix2 = glm::rotate(houseMatrix2, glm::radians(180.0f), vec3(0.0, 1.0, 1.0));
	houseMatrix2 = glm::rotate(houseMatrix2, glm::radians(45.0f), vec3(0.0, 0.0, 1.0));

	houseMatrix3 = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-10.0, terrainHeight, 17.0)), glm::vec3(1.5));
	houseMatrix3 = glm::rotate(houseMatrix3, glm::radians(180.0f), vec3(0.0, 1.0, 1.0));
	houseMatrix3 = glm::rotate(houseMatrix3, glm::radians(-90.0f), vec3(0.0, 0.0, 1.0));
	
	bigHouseMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(1.5, terrainHeight, 12.5)), glm::vec3(1.5));
	bigHouseMatrix = glm::rotate(bigHouseMatrix, glm::radians(-90.0f), vec3(1.0, 0.0, 0.0));

	//lampMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(pointLight.position.x - 1.5, terrainHeight, pointLight.position.z));
	//lampMatrix = glm::rotate(lampMatrix, glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
	towerMatrix = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, terrainHeight, 34.0f)), glm::radians(180.0f), vec3(0.0, 0.0, 1.0));
	towerMatrix = glm::rotate(towerMatrix, glm::radians(90.0f), vec3(1.0, 0.0, 0.0));
	
	castleGuardModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(-5.48f, terrainHeight, 33.245f)), glm::vec3(1.6f)); //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	castleGuardModelMatrix = glm::rotate(castleGuardModelMatrix, glm::radians(150.0f), vec3(0.0, 1.0, 0.0));
	girlMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, terrainHeight, 20.0f)), glm::vec3(1.6f)); //glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	pavementModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, terrainHeight + 0.03f, 24.0f)), glm::vec3(6.0f));
	pavementModelMatrix = glm::rotate(pavementModelMatrix, glm::radians(-20.0f), vec3(0.0, 1.0, 0.0));

	streetLightModelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, terrainHeight, 19.0f)), glm::vec3(1.5f));
	glm::vec3 lightBulbLocal = glm::vec3(0.0f, 2.6f, 0.7f);
	glm::vec3 lightBulbWorld = glm::vec3(streetLightModelMatrix * glm::vec4(lightBulbLocal, 1.0f));
	pointLight.position = lightBulbWorld;

	// Setup Lights
	lightManager->addDirectionalLight(dirLight);
	lightManager->addPointLight(pointLight);
	lightManager->applyToShader(lightingShader);
	lightSpaceMatrix = lightManager->calculateLightSpaceMatrix();
	shadowTransforms = lightManager->calculateShadowTransforms();

	// Setup Depthmap
	depthmap = new Depthmap();
	depthmap->initDepthmap();

	// Setup Skybox
	skybox = new Skybox();
	skybox->init("assets/textures/sky");
	std::cout << terrainHeight << endl;
}

void Scene::render(int window_width, int window_height, float deltaTime)
{
	vec3 cameraPos = camera->position;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// Update animation
	animator->UpdateAnimation(deltaTime);
	auto boneMatrices = animator->GetFinalBoneMatrices();
	animator2->UpdateAnimation(deltaTime);
	auto boneMatrices2 = animator2->GetFinalBoneMatrices();

	// 1. pass: render to depth map
	// ----------------------------
	depthmap->DephtmapRenderSetup();
	depthShader.activate();
	depthShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);

	// Backpack depth
	depthShader.setUniform("isAnimated", false);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	backpack->draw(depthShader);

	// Houses depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix);
	house->draw(depthShader);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix2);
	house->draw(depthShader);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix3);
	house->draw(depthShader);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, bigHouseMatrix);
	bigHouse->draw(depthShader);

	// Tower depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, towerMatrix);
	tower->draw(depthShader);

	// Pavement depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, pavementModelMatrix);
	pavement->draw(depthShader);

	// Streetlight depth
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, streetLightModelMatrix);
	streetLight->draw(depthShader);

	// Guard depth
	renderManager->setAnimated(depthShader, boneMatrices);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, castleGuardModelMatrix);
	castleGuard->draw(depthShader);

	// Girl depth
	renderManager->setAnimated(depthShader, boneMatrices2);
	depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, girlMatrix);
	girl->draw(depthShader);

	// 1. pass: render to depth cubemap
	// --------------------------------
	depthmap->CubemapRenderSetup();

	pointDepthShader.activate();
	pointDepthShader.setUniform("lightPos", pointLight.position);
	pointDepthShader.setUniform("farPlane", 25.0f); //TODO get from lightmanager pointfarplane
	for (unsigned int i = 0; i < 6; ++i)
		pointDepthShader.setUniformMatrix4fv("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, shadowTransforms[i]);

	// Backpack depth
	pointDepthShader.setUniform("isAnimated", false);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, backpackModelMatrix);
	backpack->draw(pointDepthShader);

	// House depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix);
	house->draw(pointDepthShader);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix2);
	house->draw(pointDepthShader);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, houseMatrix3);
	house->draw(pointDepthShader);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, bigHouseMatrix);
	bigHouse->draw(pointDepthShader);

	// Tower depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, towerMatrix);
	tower->draw(pointDepthShader);

	// Lantern
	//pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, lampMatrix);
	//lamp->draw(pointDepthShader);

	// Pavement depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, pavementModelMatrix);
	pavement->draw(pointDepthShader);

	// Streetlight depth
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, streetLightModelMatrix);
	streetLight->draw(pointDepthShader);

	// Guard depth
	renderManager->setAnimated(pointDepthShader, boneMatrices);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, castleGuardModelMatrix);
	castleGuard->draw(pointDepthShader);

	// Girl depth
	renderManager->setAnimated(pointDepthShader, boneMatrices2);
	pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, girlMatrix);
	girl->draw(pointDepthShader);

	// 2. pass: render scene normally with shadow mapping
	// --------------------------------------------------
	depthmap->normalRenderSetup(window_width, window_height);
	
	// Render Models with lighting
	renderManager->renderShadedModel(backpack, lightingShader, backpackModelMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(house, lightingShader, houseMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(house, lightingShader, houseMatrix2, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(house, lightingShader, houseMatrix3, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(bigHouse, lightingShader, bigHouseMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(tower, lightingShader, towerMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(pavement, lightingShader, pavementModelMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	//renderManager->renderShadedModel(lamp, lightingShader, lampMatrix, cameraPos, viewProj, lightSpaceMatrix, false);
	renderManager->renderShadedModel(streetLight, lightingShader, streetLightModelMatrix, cameraPos, viewProj, lightSpaceMatrix, false);

	renderManager->setAnimated(lightingShader, boneMatrices);
	renderManager->renderShadedModel(castleGuard, lightingShader, castleGuardModelMatrix, cameraPos, viewProj, lightSpaceMatrix, true);
	renderManager->setAnimated(lightingShader, boneMatrices2);
	renderManager->renderShadedModel(girl, lightingShader, girlMatrix, cameraPos, viewProj, lightSpaceMatrix, true);

	// Render light cube
	//renderManager->renderLightCube(lightCube, lightSourceShader, viewProj);

	// Render terrain
	TerrainRenderParams terrainParams;
	terrainParams.cameraPos = vec3(cameraPos.x, cameraPos.y - terrain->getHeightAt(cameraPos.x, cameraPos.y), cameraPos.z);
	terrainParams.minTessLevel = 0.0f;
	terrainParams.maxTessLevel = 60.0f;
	terrainParams.minDistance = 3.0f;
	terrainParams.maxDistance = 210.0f;

	renderManager->renderTerrain(terrain, terrainShader, terrainModelMatrix, view, projection, depthmap->getDepthMapTextureID(), lightSpaceMatrix, terrainParams);

	// Render skybox
	skybox->draw(skyboxShader, view, projection);
	skybox->draw(skyboxShader, view, projection);
}
#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <RenderManager.h>
#include <Lights/LightManager.h>

Scene::Scene(Camera* cam) {
	camera = cam;
	renderManager = new RenderManager();
	lightManager = new LightManager();
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

	terrain = new Terrain(terrainShader, "assets/heightmap2.png");
	camera->setTerrain(terrain);
	camera->setHeightOffset(2.0f);
	camera->position = glm::vec3(15.0f, terrain->getHeightAt(15.0, -2) + 10.0f, -2.0f);


	backpack = new Model("assets/models/backpack/backpack.obj", false);
	house = new Model("assets/models/city_house_2/city_house_2_bi.dae", true);
	tower = new Model("assets/models/Medieval tower/Medieval tower_High/Medieval tower_mid.dae", true);
	castleGuard = new Model("assets/models/castle_guard/castle_guard.dae", true);
	girl = new Model("assets/models/Peasant Girl/Peasant Girl.dae", true);
	pavement = new Model("assets/models/pavement/pavement.obj", true);
	bigHouse = new Model("assets/models/small_building_1/small_building_1.dae", true);
	lamp = new Model("assets/models/lamp/lamp1.obj", true);

	// Set textures
	tower->setTexture("assets/models/Medieval tower/Medieval tower_mid_Col.jpg", "assets/models/Medieval tower/Medieval tower_mid_spec.jpg", "assets/models/Medieval tower/Medieval tower_mid_Nor.jpg");
	lamp->setTexture("assets/models/lamp/lamp1.png", nullptr, "assets/models/lamp/lamp1normal.jpg");

	// Load Animations
	castleGuardAnimation = new Animation("assets/models/Unarmed Idle Looking Ver. 2.dae", castleGuard);
	animator = new Animator(castleGuardAnimation);
	catwalk = new Animation("assets/models/Catwalk Walk Turn 180 Tight.dae", girl);
	animator2 = new Animator(catwalk);

	// Save models
	addModel(backpack, glm::vec3(9.0f, 3.0, 21.0f), glm::vec3(0.3f), vec3(1.5708, -1.5708, 0.0));
	addModel(house, vec3(13.6365f, 0.2f, 23.45f), vec3(1.5f), vec3(-90.0f, -93.0, 0.0));
	addModel(house, vec3(5.0f, 0.0f, 30.0f), vec3(1.5f), vec3(-90.0f, -160.0f, 0.0));
	addModel(house, vec3(-10.7f, 0.0f, 16.6f), vec3(1.5f), vec3(-90.0f, -300.0f, 0.0f));
	addModel(bigHouse, vec3(1.5f, 0.0f, 12.5f), vec3(1.5f), vec3(-90.0f, 0.0f, 0.0f));
	addModel(lamp, vec3(pointLight.position.x - 1.5, 0, pointLight.position.z), vec3(1.0f), vec3(0.0f, 0.0f, 0.0f));
	addModel(tower, vec3(-11.0f, 0.0f, 34.0f), vec3(1.0f), vec3(-90.0f, -202.0f, 0.0f));
	addModel(pavement, vec3(0.0f, 0.2f, 24.0f), vec3(6.0f), vec3(0.0f, 0.0f, 0.0f), false);

	addModel(castleGuard, vec3(-5.48f, 0.0f, 33.245f), vec3(1.6f), vec3(0.0f, 150.0f, 0.0f), false, animator, true);
	addModel(girl, vec3(0.0f, 0.0f, 20.0f), vec3(1.6f), vec3(0.0f, 0.0f, 0.0f), false, animator2, true);

	// Setup Depthmap
	depthmap = new Depthmap();
	depthmap->initDepthmap();

	// Setup Skybox
	skybox = new Skybox();
	skybox->init("assets/textures/sky");

}

void Scene::render(int window_width, int window_height, float deltaTime)
{
	vec3 cameraPos = camera->position;
	glm::mat4 view = camera->getViewMatrix();
	glm::mat4 projection = camera->getProjectionMatrix();
	glm::mat4 viewProj = projection * view;

	// 1. pass: render to depth map
	// ----------------------------
	depthmap->DephtmapRenderSetup();
	depthShader.activate();
	depthShader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);

	// Render each object to depth map
	for (auto& obj : sceneObjects) {
		if (obj.isAnimated) {
			obj.animator->UpdateAnimation(deltaTime);
			auto boneMatrices = obj.animator->GetFinalBoneMatrices();
			obj.boneMatrices = &boneMatrices;
			renderManager->setAnimated(depthShader, boneMatrices);
		}
		else {
			depthShader.setUniform("isAnimated", false);
		}
		depthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, obj.transform);
		obj.model->draw(depthShader);
	}

	// 1. pass: render to depth cubemap
	// --------------------------------
	depthmap->CubemapRenderSetup();
	pointDepthShader.activate();
	pointDepthShader.setUniform("lightPos", pointLight.position);
	pointDepthShader.setUniform("farPlane", 25.0f); //TODO get from lightmanager pointfarplane
	for (unsigned int i = 0; i < 6; ++i) {
		pointDepthShader.setUniformMatrix4fv("shadowMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, shadowTransforms[i]);
	}

	// Render each object to point depth map
	for (auto& obj : sceneObjects) {
		if (obj.isAnimated) {
			auto boneMatrices = obj.animator->GetFinalBoneMatrices();
			renderManager->setAnimated(pointDepthShader, boneMatrices);
		}
		else {
			pointDepthShader.setUniform("isAnimated", false);
		}
		pointDepthShader.setUniformMatrix4fv("modelMatrix", 1, GL_FALSE, obj.transform);
		obj.model->draw(pointDepthShader);
	}

	// 2. pass: render scene normally with shadow mapping
	// --------------------------------------------------
	depthmap->normalRenderSetup(window_width, window_height);

	// Render Models with lighting
	for (auto& obj : sceneObjects) {
		if (obj.isAnimated) {
			auto boneMatrices = obj.animator->GetFinalBoneMatrices();
			renderManager->setAnimated(lightingShader, boneMatrices);
		}
		renderManager->renderShadedModel((obj.model), lightingShader, obj.transform, cameraPos, viewProj, lightSpaceMatrix, obj.isAnimated);
	}

	// Render light cube
	//renderManager->renderLightCube(lightCube, lightSourceShader, viewProj);

	// Render terrain
	TerrainRenderParams terrainParams;
	terrainParams.cameraPos = vec3(cameraPos.x, cameraPos.y - terrain->getHeightAt(cameraPos.x, cameraPos.y), cameraPos.z);
	terrainParams.minTessLevel = 0.0f;
	terrainParams.maxTessLevel = 60.0f;
	terrainParams.minDistance = 3.0f;
	terrainParams.maxDistance = 210.0f;

	renderManager->renderTerrain(terrain, terrainShader, glm::mat4(1.0f), view, projection, depthmap->getDepthMapTextureID(), lightSpaceMatrix, terrainParams);

	// Render skybox
	skybox->draw(skyboxShader, view, projection);
}

void Scene::addModel(Model* model, glm::vec3 pos, glm::vec3 scale, const glm::vec3 rot, bool movable, Animator* animator, bool isAnimated)
{
	SceneObject obj;
	obj.model = model;
	obj.pos = vec3(pos.x, terrain->getHeightAt(pos.x, pos.z) + pos.y, pos.z);
	obj.scale = scale;
	obj.rotation = rot; //Degree
	
	mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, obj.pos);
	modelMatrix = glm::scale(modelMatrix, scale);

	mat4 rotX = glm::rotate(mat4(1.0f), glm::radians(rot.x), vec3(1.0, 0.0, 0.0));
	mat4 rotY = glm::rotate(mat4(1.0f), glm::radians(rot.y), vec3(0.0, 1.0, 0.0));
	mat4 rotZ = glm::rotate(mat4(1.0f), glm::radians(rot.z), vec3(0.0, 0.0, 1.0));
	mat4 rotationMatrix = rotZ * rotY * rotX;
	modelMatrix = modelMatrix * rotationMatrix;

	obj.transform = modelMatrix;
	obj.movable = movable;
	obj.isAnimated = isAnimated;
	if (isAnimated) {
		animator->GetFinalBoneMatrices();
		obj.animator = animator;
	}
	sceneObjects.push_back(obj);
}

void Scene::translateNearestObj(float amount, glm::vec3 axis, TransformMode transformMode)
{
	if (sceneObjects.empty()) return;

	glm::vec3 camPos = camera->position;
	glm::vec3 camFront = camera->front;

	nearestObjIndex = -1;
	float minDistance = 100000.0f;
	const float focusThreshold = 0.7f;

	for (int i = 0; i < sceneObjects.size(); ++i)
	{
		if (sceneObjects[i].movable == false) continue;

		vec3 objPos = vec3(sceneObjects[i].transform[3]);
		vec3 toObjVector = objPos - camPos;
		float dist = length(toObjVector);
		toObjVector = normalize(toObjVector);
		float alignment = glm::dot(camFront, toObjVector);

		if (alignment > focusThreshold && dist < minDistance) {
			minDistance = dist;
			nearestObjIndex = i;
		}
	}

	if (nearestObjIndex >= 0) {
		if (transformMode == ROTATE) {
			sceneObjects[nearestObjIndex].rotation += axis * amount;
			vec3 rot = sceneObjects[nearestObjIndex].rotation;

			mat4 R =
				rotate(mat4(1.0f), radians(rot.z), vec3(0.0, 0.0, 1.0)) *
				rotate(mat4(1.0f), radians(rot.y), vec3(0.0, 1.0, 0.0)) *
				rotate(mat4(1.0f), radians(rot.x), vec3(1.0, 0.0, 0.0));
			mat4 M = translate(mat4(1.0f), sceneObjects[nearestObjIndex].pos);
			M = scale(M, sceneObjects[nearestObjIndex].scale);

			sceneObjects[nearestObjIndex].transform = M * R;
		}
		else if (transformMode == TRANSLATE)
			sceneObjects[nearestObjIndex].pos += axis * amount;
			sceneObjects[nearestObjIndex].transform = glm::translate(sceneObjects[nearestObjIndex].transform, axis * amount);
	}

}

glm::vec3 Scene::getPositionOfLastObject()
{
	if (nearestObjIndex < 0 || nearestObjIndex >= sceneObjects.size()) {
		return glm::vec3();
	}
	glm::vec3 pos = sceneObjects[nearestObjIndex].transform[3];
	return vec3(pos.x, pos.y - terrain->getHeightAt(pos.x, pos.z), pos.z);
}

glm::vec3 Scene::getRotationOfLastObject()
{
	if (nearestObjIndex < 0 || nearestObjIndex >= sceneObjects.size()) {
		return glm::vec3();
	}

	glm::vec3 rotation = sceneObjects[nearestObjIndex].rotation;

	return rotation;
}

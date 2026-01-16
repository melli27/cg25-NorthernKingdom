// Scene.h
#pragma once
#include "Shader.h"
#include "Model/Model.h"
#include "Model/Geometry.h"
#include "Terrain/Terrain.h"
#include "Camera.h"
#include "Lights/LightManager.h"
#include "RenderManager.h"

class Scene {
public:
    Scene(Camera* camera);
	~Scene();

    void init();
    void render(float deltaTime);
    void update(float deltaTime);

private:
    Camera* camera;

    RenderManager* renderManager;
	LightManager* lightManager;

    Shader lightSourceShader;
    Shader lightingShader;
    Shader terrainShader;

    Terrain* terrain;
    Model* backpack;
	Model* castleGuard;
    Geometry* lightCube;

    glm::mat4 backpackModelMatrix;
	glm::mat4 castleGuardModelMatrix;
    glm::mat4 terrainModelMatrix;
};
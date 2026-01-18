// Scene.h
#pragma once
#include "Shader.h"
#include "Model/Model.h"
#include "Model/Geometry.h"
#include "Terrain/Terrain.h"
#include "Camera.h"
#include "Lights/LightManager.h"
#include "RenderManager.h"
#include <Depthmap.h>
#include <Skybox/Skybox.h>
#include <Animator.h>

class Scene {
public:
    Scene(Camera* camera);
	~Scene();

    void init();
    void render(int window_width, int window_height, float deltaTime);
    void update(float deltaTime);

private:
    Camera* camera;

    RenderManager* renderManager;
	LightManager* lightManager;

    Shader lightSourceShader;
    Shader lightingShader;
    Shader lightingShader2;
    Shader depthShader;
	Shader pointDepthShader;
	Shader skyboxShader;

    Shader terrainShader;
    Shader animatedModelShader;

	mat4 lightSpaceMatrix;
	vector<mat4> shadowTransforms;
	Depthmap* depthmap;

    Terrain* terrain;
    Model* backpack;
    Model* house;
	Model* castleGuard;
    Geometry* lightCube;
    Geometry* testCube;
	Skybox* skybox;

	Animation* castleGuardAnimation;
    Animator* animator;

    glm::mat4 backpackModelMatrix;
	glm::mat4 castleGuardModelMatrix;
    glm::mat4 terrainModelMatrix;
    glm::mat4 houseMatrix;

    DirectionalLight dirLight{
        glm::vec3(-0.4f, -0.6f, -0.2f),  // direction
        glm::vec3(1.0f, 0.9f, 0.7f),    // diffuse
        glm::vec3(0.2f, 0.2f, 0.2f),    // ambient
        glm::vec3(0.9f, 0.6f, 0.4f),    // specular
        glm::vec3(0.9f, 0.9f, 0.8f)     // color? (check your ctor)
    };

    PointLight pointLight{
        glm::vec3(-3.0f, -1.0f, -3.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),    // diffuse
        glm::vec3(0.2f, 0.2f, 0.2f),    // ambient
        glm::vec3(0.5f, 0.5f, 0.5f),    // specular
        glm::vec3(1.0f, 1.0f, 1.0f),    // color?
        glm::vec3(1.0f, 0.09f, 0.032f)  // attenuation
    };
};
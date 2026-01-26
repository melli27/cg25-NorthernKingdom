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
#include <InputManager.h>

struct SceneObject {
    Model* model;
    mat4 transform = glm::mat4(1.0f);
    vec3 pos = vec3(0.0f);
    vec3 rotation = vec3(0.0f);
    vec3 scale = vec3(1.0f);
	bool isAnimated = false;
    Animator* animator; // Optional; nullptr if not animated
    std::vector<mat4>* boneMatrices; // Optional; empty if not animated
	bool movable = true;
};

class Scene {
public:
    Scene(Camera* camera);
    ~Scene();

    void init();
    void render(int window_width, int window_height, float deltaTime);
    
    void addModel(Model* model, glm::vec3 pos, glm::vec3 scale, vec3 rotate, bool movable = true, Animator* animator = nullptr, bool isAnimated = false);

	void translateNearestObj(float amount, glm::vec3 axis, TransformMode transformMode);

	glm::vec3 getPositionOfLastObject();

	glm::vec3 getRotationOfLastObject();

    glm::vec3 extractEulerAngles(const glm::mat4& m);
    
private:
    Camera* camera;

    RenderManager* renderManager;
    LightManager* lightManager;

    vector <SceneObject> sceneObjects;
	int nearestObjIndex = -1;

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
    Model* bigHouse;
    Model* bench;
    Model* house;
    Model* tower;
    Model* castleGuard;
    Model* girl;
    Model* lamp;
    Model* pavement;
    Geometry* lightCube;
    Geometry* testCube;
    Skybox* skybox;

    Animation* castleGuardAnimation;
    Animator* animator;

    Animation* catwalk;
    Animator* animator2;

    glm::mat4 backpackModelMatrix;
    glm::mat4 castleGuardModelMatrix;
    glm::mat4 girlMatrix;
    glm::mat4 benchMatrix;
    glm::mat4 terrainModelMatrix;
    glm::mat4 houseMatrix;
    glm::mat4 houseMatrix2;
    glm::mat4 houseMatrix3;
    glm::mat4 bigHouseMatrix;
    glm::mat4 towerMatrix;
    glm::mat4 lampMatrix;
    glm::mat4 pavementModelMatrix;

    DirectionalLight dirLight{
        glm::vec3(-0.4f, -0.6f, -0.2f),  // direction
        glm::vec3(1.0f, 0.9f, 0.7f),    // diffuse
        glm::vec3(0.2f, 0.2f, 0.2f),    // ambient
        glm::vec3(0.9f, 0.6f, 0.4f),    // specular
        glm::vec3(0.9f, 0.9f, 0.8f)     // color? (check your ctor)
    };

    PointLight pointLight{
        glm::vec3(7.0f, -81, 21.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),    // diffuse
        glm::vec3(0.2f, 0.2f, 0.2f),    // ambient
        glm::vec3(0.5f, 0.5f, 0.5f),    // specular
        glm::vec3(1.0f, 1.0f, 1.0f),    // color?
        glm::vec3(1.0f, 0.09f, 0.032f)  // attenuation
    };

    void initModels();
};
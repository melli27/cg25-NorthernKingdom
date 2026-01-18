#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Model/Model.h"
#include "Model/Geometry.h"
#include "Terrain/Terrain.h"
#include <glm/glm.hpp>
#include <Depthmap.h>

struct TerrainRenderParams {
    glm::vec3 cameraPos;
    float minDistance;
    float maxDistance;
    float minTessLevel;
    float maxTessLevel;
};

class RenderManager {
public:
    RenderManager();
    ~RenderManager();

    // Render different object types
    //void renderDepthmap(Depthmap* depthmap, Shader& shader, mat4 lightSpaceMatrix, map<Model*,mat4> modelMatrixMap);

    void renderModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPostion, const glm::mat4& viewProj);

    void renderShadedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPosition, const glm::mat4& viewProj, const mat4& lightspaceMatrix, bool isAnimated);
    
	void setAnimated(Shader& shader, std::vector<glm::mat4>& transformationMatrices);

    void renderAnimatedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, vec3 cameraPosition, const glm::mat4& viewProj, vector<mat4>& transformationMatrices);

    void renderLightCube(Geometry* lightCube, Shader& shader, const glm::mat4& viewProj);

    void renderTerrain(Terrain* terrain, Shader& shader, const glm::mat4& modelMatrix,
        const glm::mat4& view, const glm::mat4& projection,
        const TerrainRenderParams& params);
};
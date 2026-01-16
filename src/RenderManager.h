#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Model/Model.h"
#include "Model/Geometry.h"
#include "Terrain/Terrain.h"
#include <glm/glm.hpp>

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
    void renderModel(Model* model, Shader& shader, const glm::mat4& modelMatrix,
        Camera* camera, const glm::mat4& viewProj);

    void renderAnimatedModel(Model* model, Shader& shader, const glm::mat4& modelMatrix, Camera* camera, const glm::mat4& viewProj);

    void renderLightCube(Geometry* lightCube, Shader& shader, const glm::mat4& viewProj);

    void renderTerrain(Terrain* terrain, Shader& shader, const glm::mat4& modelMatrix,
        const glm::mat4& view, const glm::mat4& projection,
        const TerrainRenderParams& params);
};
#pragma once

#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Shader.h"
#include <vector>
using namespace glm;

class LightManager {
public:
    LightManager();
    ~LightManager();

    void addDirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);

    void applyToShader(Shader& shader);

    mat4 calculateLightSpaceMatrix() const;

    std::vector<mat4> calculateShadowTransforms() const;

private:
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
};
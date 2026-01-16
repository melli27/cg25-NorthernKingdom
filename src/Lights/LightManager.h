#pragma once

#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Shader.h"
#include <vector>

class LightManager {
public:
    LightManager();
    ~LightManager();

    void addDirectionalLight(const DirectionalLight& light);
    void addPointLight(const PointLight& light);

    void applyToShader(Shader& shader);

private:
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
};
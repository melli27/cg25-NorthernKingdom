#include "LightManager.h"

LightManager::LightManager() {
}

LightManager::~LightManager() {
}

void LightManager::addDirectionalLight(const DirectionalLight& light) {
    directionalLights.push_back(light);
}

void LightManager::addPointLight(const PointLight& light) {
    pointLights.push_back(light);
}

void LightManager::applyToShader(Shader& shader) {
    shader.activate();

    // Apply directional lights (assuming single light for simplicity)
    if (!directionalLights.empty()) {
        directionalLights[0].applyToShader(shader, "dirLight");
    }

    // Apply point lights (assuming single light for simplicity)
    if (!pointLights.empty()) {
        pointLights[0].applyToShader(shader, "pointLight");
    }
}
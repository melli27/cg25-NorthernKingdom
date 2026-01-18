#pragma once
#include "Shader.h"
#include <Lights/DirectionalLight.h>
#include <Lights/PointLight.h>

using namespace glm;

class Depthmap 
{

public:
    Depthmap();
    ~Depthmap();

	void initDepthmap();

    // Render different object types
    void DephtmapRenderSetup();
	void CubemapRenderSetup();
	void normalRenderSetup(int window_width, int window_height);

	unsigned int getDepthMapTextureID() const { return depthMap; }

private:
	const unsigned int SHADOW_WIDTH = 2048;
	const unsigned int SHADOW_HEIGHT = 2048;

	// Directional depth map FBO
	unsigned int depthMapFBO;
	unsigned int depthMap; // depth texture

	// Point depth map FBO
	const unsigned int POINT_SHADOW_SIZE = 1024;
	unsigned int depthCubeMapFBO;
	unsigned int depthCubemap; // depth cubemap texture

	// TODO adapt to scence size
	float nearPlane = 1.0f;
	float farPlane = 40.0f;
	float orthoSize = 10.0f;

};
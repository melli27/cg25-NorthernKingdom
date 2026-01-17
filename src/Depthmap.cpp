#include "Depthmap.h"

Depthmap::Depthmap() 
{
}

void Depthmap::initDepthmap()
{
	// Directional depth map FBO
	glGenFramebuffers(1, &depthMapFBO);

	// depth texture
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Point depth map FBO
	glGenFramebuffers(1, &depthCubeMapFBO);
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// attach to FBO
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Depthmap::renderToDepthmap()
{
	// 1. pass: render to depth map
	// ----------------------------
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void Depthmap::renderToDepthCubemap()
{
	// 1. pass: render to depth cubemap
	// --------------------------------
	glViewport(0, 0, POINT_SHADOW_SIZE, POINT_SHADOW_SIZE);
	glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
}

void Depthmap::normalRenderSetup(int window_width, int window_height)
{
	glViewport(0, 0, window_width, window_height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const int dirShadowUnit = 3;
	glActiveTexture(GL_TEXTURE0 + dirShadowUnit);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	const int pointShadowUnit = 4;
	glActiveTexture(GL_TEXTURE0 + pointShadowUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
}

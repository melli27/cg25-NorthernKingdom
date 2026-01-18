#pragma once
#include <string>
#include <Utils/Utils.h>
#include <Shader.h>

class Skybox {
public:
	Skybox();

	~Skybox();

	void init(const std::string& folderPath);
	void draw(Shader& skyboxShader, const glm::mat4& view, const glm::mat4& projection);
	
	unsigned int getCubemapTexture() const { return cubemapTexture; }

private:
	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int cubemapTexture;

	unsigned int loadCubemap(const std::vector<std::string>& faces);
};
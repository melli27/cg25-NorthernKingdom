#include "Terrain.h"

Terrain::Terrain(Shader& shader, const char* heightMapPath)
{
	shader.activate();

	heightMapTexture.loadFromFile(heightMapPath);
	heightMapTexture.bind(0);
	shader.setUniform("heightMap", 0);

	//Rock
	rockDiffuse.loadFromFile("assets/textures/rock/rock_diffuse.dds"); //TODO increase loading speed (lower pic quality or no normal or rgb only)
	shader.setUniform("rockDiffuse", 1);

	rockNormal.loadFromFile("assets/textures/rock/rock_normal.dds");
	shader.setUniform("rockNormal", 2);

	//Grass
	grassDiffuse.loadFromFile("assets/textures/grass/grass_diffuse.dds");
	shader.setUniform("grassDiffuse", 3);

	grassNormal.loadFromFile("assets/textures/grass/grass_normal.dds");
	shader.setUniform("grassNormal", 4); 

	//Snow
	snowDiffuse.loadFromFile("assets/textures/snow/snow_diffuse.dds");
	shader.setUniform("snowDiffuse", 5);

	snowNormal.loadFromFile("assets/textures/snow/snow_normal.dds");
	shader.setUniform("snowNormal", 6);

	shader.setUniform("lightDir", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setUniform("viewPos", glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)));

	height = heightMapTexture.height * terrainScale;
	width = heightMapTexture.width * terrainScale;

	// Set up vertex data

	for (unsigned i = 0; i <= rez - 1; i++)
	{
		for (unsigned j = 0; j <= rez - 1; j++)
		{
			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back(j / (float)rez); // v
			//vertices.push_back(i / (float)rez); // u

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * j / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back(j / (float)rez); // v
			//vertices.push_back((i + 1) / (float)rez); // u

			vertices.push_back(-width / 2.0f + width * i / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back(i / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
			//vertices.push_back(i / (float)rez); // u

			vertices.push_back(-width / 2.0f + width * (i + 1) / (float)rez); // v.x
			vertices.push_back(0.0f); // v.y
			vertices.push_back(-height / 2.0f + height * (j + 1) / (float)rez); // v.z
			vertices.push_back((i + 1) / (float)rez); // u
			vertices.push_back((j + 1) / (float)rez); // v
			//vertices.push_back((i + 1) / (float)rez); // u
		}
	}
	std::cout << "Loaded " << rez * rez << " patches of 4 control points each" << std::endl;
	std::cout << "Processing " << rez * rez * 4 << " vertices in vertex shader" << std::endl;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

}

float Terrain::getHeightAt(float worldX, float worldZ) const {
	// Convert world coordinates to heightmap UV coordinates
	float u = (worldX + width / 2.0f) / width;
	float v = (worldZ + height / 2.0f) / height;

	// Clamp to valid range
	u = glm::clamp(u, 0.0f, 1.0f);
	v = glm::clamp(v, 0.0f, 1.0f);

	int x = (int)(u * heightMapTexture.width);
	int y = (int)(v * heightMapTexture.height);

	// transformation from tessellation shader:
	// Height = texture(heightMap, uv).r * 200.0 - 100.0
	float heightValue = heightMapTexture.getPixelValue(x,y); // Get from heightmap data
	return heightValue * 100.0f - 100.0f;
}


void Terrain::Draw(Shader& shader, unsigned int depthMapTexture, const glm::mat4& lightSpaceMatrix)
{
	shader.activate();

	glActiveTexture(GL_TEXTURE0);
	heightMapTexture.bind(0);

	glActiveTexture(GL_TEXTURE1);
	rockDiffuse.bind(1);

	glActiveTexture(GL_TEXTURE2);
	rockNormal.bind(2);
	
	glActiveTexture(GL_TEXTURE3);
	grassDiffuse.bind(3);

	glActiveTexture(GL_TEXTURE4);
	grassNormal.bind(4);

	glActiveTexture(GL_TEXTURE5);
	snowDiffuse.bind(5);

	glActiveTexture(GL_TEXTURE6);
	snowNormal.bind(6);
	
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	shader.setUniform("shadowMap", 7);
	shader.setUniformMatrix4fv("lightSpaceMatrix", 1, GL_FALSE, lightSpaceMatrix);

	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void Terrain::deleteTerrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
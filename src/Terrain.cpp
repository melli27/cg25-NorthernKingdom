#include "Terrain.h"

Terrain::Terrain(Shader& shader, const char* texturePath, const char* heightMapPath)
{
	shader.activate();

	heightMapTexture.loadFromFile(heightMapPath);
	//heightMapTexture.bind(0);
	shader.setUniform1i("heightMap", 0);
	//surfaceTexture.loadFromFile(texturePath);

	height = heightMapTexture.height;
	width = heightMapTexture.width;

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

	//surfaceTexture.bind(1);
	//surfaceTexture.loadFromFile(texturePath);
	//shader.setUniform1i("surfaceTexture", 1);

} 

void Terrain::Draw(Shader& shader)
{
	shader.activate();

	//glActiveTexture(GL_TEXTURE0);
	// 
	heightMapTexture.bind(0);

	//surfaceTexture.bind(1);
	//shader.setUniform1i("surfaceTexture", 1);

	glBindVertexArray(VAO);
	glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
}

void Terrain::deleteTerrain() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
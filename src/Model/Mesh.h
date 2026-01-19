#pragma once
#include "Utils/Utils.h"
#include <Shader.h>
#include <Model/Texture.h>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords; //uv coordinates

	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	int boneIDs[MAX_BONE_INFLUENCE] = { -1, -1, -1, -1 };
	float weights[MAX_BONE_INFLUENCE] = { 0.0f, 0.0f, 0.0f, 0.0f };
};

class Mesh {
public:

	vector<Vertex>vertices;
	vector<unsigned int> indices;
	//vector<BoneData> boneData;

	vector<shared_ptr<Texture>> textures;

	float shininess = 32.0f;

	unsigned int VAO;

	// constructor
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<shared_ptr<Texture>> textures);

	// render the mesh
	void Draw(Shader& shader);

private:
	// render data 
	unsigned int VBO, EBO;
	GLuint bonesVBO;
	GLuint weightsVBO;

	// initializes all the buffer objects/arrays
	void setupMesh();
};
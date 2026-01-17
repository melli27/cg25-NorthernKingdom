#include "Mesh.h"

#define POSITION_LOCATION	0
#define NORMAL_LOCATION		1
#define TEX_COORD_LOCATION	2
#define TANGENT_LOCATION	3
#define BITANGENT_LOCATION	4
#define BONE_ID_LOCATION	5
#define BONE_WEIGHT_LOCATION 6

Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<shared_ptr<Texture>> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

void Mesh::Draw(Shader &shader)
{
	//glBindVertexArray(VAO); //TODO needed?
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		string name = textures[i]->type;

		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i]->handle);

		string uniformName;

		if (name == "diffuseTexture")
			uniformName = "material.diffuseTexture";
		else if (name == "specularTexture")
			uniformName = "material.specularTexture";
		else if (name == "normalTexture")
			uniformName = "material.normalTexture";
		else
			uniformName = "material.diffuseTexture";
		
		glUniform1i(glGetUniformLocation(shader.ID, uniformName.c_str()), i);
	}

	shader.setUniform("material.shininess", shininess);

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}

void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// bind positions to loction 0
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	// bind normals to location 1
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

	// create uv/ vertex texture coords
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	
	// vertex tangent
	glEnableVertexAttribArray(TANGENT_LOCATION);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	
	// vertex bitangent
	glEnableVertexAttribArray(BITANGENT_LOCATION);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
	// bone
	//ids
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, boneIDs));

	// weights
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

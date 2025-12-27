#pragma once

#include "Utils/Utils.h"
#include <Mesh.h>

using namespace std;

class Terrain {

public:
	int verticesCount;

	Terrain(Shader& shader, const char* texturePath, const char* heightMapPath);
	void Draw(Shader& shader);
	void deleteTerrain(); 

private:
	//TODO maybe shift in mesh.h
	Texture heightMapTexture;
	Texture surfaceTexture;

	Texture rockDiffuse;
	Texture rockNormal;

	Texture grassDiffuse;
	Texture grassNormal;

	Texture snowDiffuse;
	Texture snowNormal;

	vector<float> vertices;
	unsigned int VAO, VBO;
	unsigned int NUM_PATCH_PTS = 4;
	unsigned rez = 64;
	float terrainScale = 0.5f;
	int width, height; // TODO , nrChannels;

};
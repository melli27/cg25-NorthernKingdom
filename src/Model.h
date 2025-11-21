#pragma once
#include "Utils/Utils.h"
#include <Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

using namespace std;

class Model
{
public:
	// model data 
	vector<Mesh> meshes;
	vector<glm::vec3> positions;
	
	string directory;
	bool gammaCorrection;

	vector<std::shared_ptr<Texture>> loadedTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	vector<string> loadedTexturePaths;

	Model(string const& path, bool gamma = false);
	void Draw(Shader& shader);

private:
	const aiScene* scene = nullptr;
	string folderName;
	void loadModel(string const& path);
	void processNode(aiNode* node);
	Mesh processMesh(aiMesh* mesh);
	void loadMaterialTextures(aiMesh* mesh, vector<shared_ptr<Texture>>& meshTextures);
	
	vector<shared_ptr<Texture>> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const string& typeName);

};
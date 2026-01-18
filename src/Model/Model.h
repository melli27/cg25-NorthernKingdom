#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "Utils/Utils.h"
#include <Model/Mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

using namespace std;
using namespace glm;

struct BoneInfo
{
	int id;
	glm::mat4 offset;
	glm::mat4 finalTransformation;
};

class Model
{
public:

	Model(string const& path, bool flipUVs);
	void draw(Shader& shader);

	//map<string, BoneInfo> getBoneInfoMap() const { return boneInfoMap; }
	vector<mat4> getBoneTransforms(float timeInSeconds, mat4 globalTransform);

	const aiScene* getScene() const { return scene; }
	map<string, BoneInfo>& getBoneInfoMap() { return boneInfoMap; }
	unsigned int& getBoneCount() { return boneCounter; }

private:

	unsigned int test = 0;
	Assimp::Importer importer;
	const aiScene* scene = nullptr;
	const aiAnimation* animation = nullptr;

	// model data 
	string directory;
	vector<Mesh> modelMeshes;
	vector<vec3> positions;
	vector<shared_ptr<Texture>> loadedTextures;
	vector<string> loadedTexturePaths;
	
	// bone data
	map<string, BoneInfo> boneInfoMap;
	unsigned int boneCounter = 0; // 36

	// animation data
	vector<mat4> finalBoneMatrices;

	// model loading
	void loadModel(string const& path, bool flipUVs);
	void processNode(aiNode* node);
	Mesh processMesh(aiMesh* mesh);
	void processBones(vector<Vertex>& vertices, aiMesh* mesh);
	void loadMaterialTextures(aiMesh* mesh, vector<shared_ptr<Texture>>& meshTextures);
	glm::mat4 convertAiMatrixToGlm(const aiMatrix4x4& from);
};
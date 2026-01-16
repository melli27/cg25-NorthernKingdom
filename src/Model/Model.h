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

	Model(string const& path);
	void draw(Shader& shader);
	void animate(Shader& shader, mat4 modelMatrix, float currentTime, vec3 dir, float speed, float dt);

	vector<mat4> getBoneTransforms(float timeInSeconds, mat4 globalTransform);

	const aiScene* getScene() const { return scene; }
	std::map<std::string, BoneInfo>& getBoneInfoMap() { return boneInfoMap; }

private:

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
	unsigned int boneCounter = 0;

	// animation data
	aiMatrix4x4 globalInverseTransform;
	vector<mat4> finalBoneMatrices;

	// model loading
	void loadModel(string const& path);
	void processNode(aiNode* node);
	Mesh processMesh(aiMesh* mesh);
	void loadMaterialTextures(aiMesh* mesh, vector<shared_ptr<Texture>>& meshTextures);
	void readNodeHierarchy(float animationTimeTicks, const aiNode* node, const mat4& parentTransform);
	
	// animation load
	const aiNodeAnim* findNodeAnim(const aiAnimation* animation, const string& nodeName);
	glm::mat4 CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	glm::mat4 CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	glm::mat4 CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	glm::mat4 convertAiMatrixToGlm(const aiMatrix4x4& from);
};
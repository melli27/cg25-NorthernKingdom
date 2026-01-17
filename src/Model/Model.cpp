#include "Model.h"

Model::Model(string const& path)
{
	loadModel(path);
}

void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < modelMeshes.size(); i++) {
		modelMeshes[i].Draw(shader);
	}
}

void Model::animate(Shader& shader, mat4 modelMatrix, float currentTime, vec3 dir, float speed, float dt)
{
	vector<glm::mat4> transformationMatrices = getBoneTransforms(currentTime, mat4(1.0f));
	for (unsigned int i = 0; i < transformationMatrices.size(); i++) {
		glm::mat4 mat = transformationMatrices[i];
		shader.setUniformMatrix4fv("boneMatrices[" + std::to_string(i) + "]", 1, GL_FALSE, mat);
	}
}


void Model::loadModel(string const& path)
{
	scene = importer.ReadFile(
		path,
		aiProcess_Triangulate |
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace | //for tangents and bitangents
		aiProcess_JoinIdenticalVertices
		//aiProcess_FlipUVs
	);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	std::cout << "Model directory: " << directory << endl;

	processNode(scene->mRootNode);
}

void Model::processNode(aiNode* node)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		modelMeshes.push_back(processMesh(mesh));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i]);
	}
}

Mesh Model::processMesh(aiMesh* mesh)
{
	// data to fill
	vector<Vertex> verts;
	vector<unsigned int> indices;
	vector<shared_ptr<Texture>> meshTextures;

	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex v;

		aiVector3D& pos = mesh->mVertices[i];

		// position
		v.Position = glm::vec3(pos.x, pos.y, pos.z);

		// normal
		if (mesh->mNormals)
		{
			aiVector3D& normal = mesh->mNormals[i];
			v.Normal = glm::vec3(normal.x, normal.y, normal.z);
		}
		else {
			v.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		// texcoords
		if (mesh->mTextureCoords[0]) {
			aiVector3D& texCoord = mesh->mTextureCoords[0][i];
			v.TexCoords = glm::vec2(texCoord.x, texCoord.y);
		}
		else {
			v.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		// tangents & bitangents
		if (mesh->mTangents && mesh->mBitangents)
		{
			aiVector3D& tangent = mesh->mTangents[i];
			v.Tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
			aiVector3D& bitangent = mesh->mBitangents[i];
			v.Bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
		}
		else {
			v.Tangent = glm::vec3(0.0f);
			v.Bitangent = glm::vec3(0.0f);
		}

		//initialize bone data
		for (int j = 0; j < MAX_BONE_INFLUENCE; j++)
		{
			v.boneIDs[j] = 0;
			v.weights[j] = 0.0f;
		}

		positions.push_back(v.Position);
		verts.push_back(v);
	}

	// bones
	for (unsigned int i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		string boneName(bone->mName.C_Str());
		int boneID = -1;

		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneCounter++;
			BoneInfo newBoneInfo;
			boneID = (int)boneInfoMap.size();
			newBoneInfo.id = boneID;
			newBoneInfo.offset = convertAiMatrixToGlm(bone->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}

		auto weights = bone->mWeights;

		// assign weights to vertices
		for (unsigned int j = 0; j < bone->mNumWeights; j++)
		{
			int vertexID = weights[j].mVertexId;
			float weightValue = weights[j].mWeight;

			for (int k = 0; k < MAX_BONE_INFLUENCE; k++)
			{
				if (verts[vertexID].weights[k] == 0.0f)
				{
					verts[vertexID].boneIDs[k] = boneID;
					verts[vertexID].weights[k] = weightValue;
					break;
				}
			}
		}
	}

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// material textures
	loadMaterialTextures(mesh, meshTextures);

	//create mesh object
	Mesh outMesh(verts, indices, meshTextures);

	// material shininess
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	float shininessValue = 32.0f;
	if (material)
	{
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, shininessValue))
			outMesh.shininess = shininessValue;
		else
			outMesh.shininess = 32.0f;
	}

	return outMesh;
}

void Model::loadMaterialTextures(aiMesh* mesh, std::vector<std::shared_ptr<Texture>>& meshTextures)
{

	if (!scene || !mesh) return;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (!material) return;

	aiTextureType types[3] = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS }; //TODO add more?
	string typeNames[3] = { "diffuseTexture", "specularTexture", "normalTexture" };

	for (int i = 0; i < 3; i++)
	{
		aiTextureType type = types[i];
		const string& typeName = typeNames[i];
		for (unsigned int j = 0; j < material->GetTextureCount(type); j++)
		{
			aiString str;
			material->GetTexture(type, j, &str);

			string filePath = directory + '/' + string(str.C_Str());
			bool skip = false;

			for (unsigned int k = 0; k < loadedTexturePaths.size(); k++)
			{
				if (loadedTexturePaths[k] == filePath)
				{
					meshTextures.push_back(loadedTextures[k]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				// Create new texture
				shared_ptr<Texture> tex = make_shared<Texture>();
				tex->loadFromFile(filePath.c_str());
				tex->type = typeName;

				loadedTextures.push_back(tex);
				loadedTexturePaths.push_back(filePath);
				meshTextures.push_back(tex);
			}
		}
	}

	// If no diffuse texture found, use default texture TODO
	if (material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
	{
		cout << "No diffuse texture found for mesh. Using default texture" << endl;

		shared_ptr<Texture> tex = make_shared<Texture>();
		tex->loadFromFile("src/diffuse.jpg");
		tex->type = "diffuseTexture";

		meshTextures.push_back(tex);
	}
}

vector<mat4> Model::getBoneTransforms(float timeInSeconds, mat4 globalTransform)
{
	finalBoneMatrices.resize(boneCounter);
	if (scene && scene->mNumAnimations > 0)
	{
		if (animation == nullptr)
		{
			animation = scene->mAnimations[0]; // Mixamorig_Hips
		}
		float ticksPerSecond = animation->mTicksPerSecond;
		float timeInTicks = timeInSeconds * ticksPerSecond;
		float animationTimeTicks = fmod(timeInTicks, animation->mDuration);

		readNodeHierarchy(animationTimeTicks, scene->mRootNode, mat4(1.0f));
	}
	else
	{
		for (unsigned int i = 0; i < finalBoneMatrices.size(); i++)
		{
			finalBoneMatrices[i] = mat4(1.0f);
		}
	}
	return finalBoneMatrices;
}

void Model::readNodeHierarchy(float animationTimeTicks, const aiNode* node, const mat4& parentTransform)
{
	string nodeName = node->mName.data;
	mat4 nodeTransform = convertAiMatrixToGlm(node->mTransformation);
	const aiNodeAnim* nodeAnim = findNodeAnim(animation, node->mName.data);

	if (nodeAnim)
	{
		mat4 scalingMat = CalcInterpolatedScaling(animationTimeTicks, nodeAnim);
		mat4 rotationMat = CalcInterpolatedRotation(animationTimeTicks, nodeAnim);
		mat4 translationMat = CalcInterpolatedPosition(animationTimeTicks, nodeAnim);
		nodeTransform = translationMat * rotationMat * scalingMat;
	}

	mat4 globalTransform = parentTransform * nodeTransform;
	aiMatrix4x4 globalInverseTransform = scene->mRootNode->mTransformation;//.Inverse();

	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int boneID = boneInfoMap[nodeName].id;
		boneInfoMap[nodeName].finalTransformation = /*convertAiMatrixToGlm(globalInverseTransform)* */ globalTransform * boneInfoMap[nodeName].offset;
		finalBoneMatrices[boneID] = /*convertAiMatrixToGlm(globalInverseTransform) **/  globalTransform * boneInfoMap[nodeName].offset;
		if (test < 2) {
			cout << "Bone name: " << nodeName << endl;
			cout << "Bone ID: " << boneID << endl;
			cout << "Final transformation: " << glm::to_string(boneInfoMap[nodeName].finalTransformation) << endl;
		}
	}

	if (test == 0) {
		test++;
		cout << "parentTransform: " << glm::to_string(parentTransform) << endl;
		cout << "nodeTransform: " << glm::to_string(nodeTransform) << endl;
		cout << "globalTransform: " << glm::to_string(globalTransform) << endl;
		cout << "globalInverseTransform: " << glm::to_string(globalTransform) << endl;
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		readNodeHierarchy(animationTimeTicks, node->mChildren[i], globalTransform);
	}
}

const aiNodeAnim* Model::findNodeAnim(const aiAnimation* animation, const string& nodeName)
{
	for (unsigned int i = 0; i < animation->mNumChannels; i++) {
		const aiNodeAnim* nodeAnim = animation->mChannels[i];

		if (nodeAnim->mNodeName.data == nodeName) {
			return nodeAnim;
		}
	}
	return nullptr;
}

glm::mat4 Model::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		aiVector3D v = pNodeAnim->mScalingKeys[0].mValue;
		glm::vec3 vec = glm::vec3(v.x, v.y, v.z);
		return glm::scale(glm::mat4(1.0f), vec);
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = ScalingIndex + 1;
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);

	float t1 = (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float t2 = (float)pNodeAnim->mScalingKeys[NextScalingIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTime - (float)t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);

	aiVector3D firstSc = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	glm::vec3 scale1 = glm::vec3(firstSc.x, firstSc.y, firstSc.z);
	aiVector3D secondSc = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	glm::vec3 scale2 = glm::vec3(secondSc.x, secondSc.y, secondSc.z);

	glm::vec3 finalScale = glm::mix(scale1, scale2, Factor);

	return glm::scale(glm::mat4(1.0f), finalScale);
}

glm::mat4 Model::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	return mat4(1.0f);
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		return glm::mat4(1.0f);
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = RotationIndex + 1;
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

	float t1 = (float)pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float t2 = (float)pNodeAnim->mRotationKeys[NextRotationIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTime - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);

	aiQuaternion StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	glm::quat startrot(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
	aiQuaternion EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	glm::quat endrot(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

	glm::quat finalRotation = glm::slerp(startrot, endrot, Factor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 Model::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) {
		return glm::mat4(1.0f);
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = PositionIndex + 1;
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys); //140 pos keys

	float t1 = (float)pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float t2 = (float)pNodeAnim->mPositionKeys[NextPositionIndex].mTime;
	float DeltaTime = t2 - t1;
	float Factor = (AnimationTime - t1) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);

	const aiVector3D Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	glm::vec3 startp(Start.x, Start.y, Start.z);
	const aiVector3D End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	glm::vec3 endp(End.x, End.y, End.z);

	glm::vec3 finalPos = glm::mix(startp, endp, Factor);
	return glm::translate(glm::mat4(1.0f), finalPos);
}

unsigned int Model::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		float t = (float)pNodeAnim->mScalingKeys[i + 1].mTime;
		if (AnimationTime < t) {
			return i;
		}
	}

	return 0;
}

unsigned int Model::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		float t = (float)pNodeAnim->mRotationKeys[i + 1].mTime;
		if (AnimationTime < t) {
			return i;
		}
	}

	return 0;
}

unsigned int Model::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		float t = (float)pNodeAnim->mPositionKeys[i + 1].mTime;
		if (AnimationTime < t) {
			return i;
		}
	}

	return 0;
}

glm::mat4 Model::convertAiMatrixToGlm(const aiMatrix4x4& from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

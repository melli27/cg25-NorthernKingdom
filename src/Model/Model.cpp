#include "Model.h"

Model::Model(string const& path, bool flipUVs)
{
	loadModel(path, flipUVs);
}

void computeTangentsBitangents(vector<Vertex>& vertices, vector<unsigned int>& indices)
{
	for (int i = 0; i + 2 < indices.size(); i += 3) {
		Vertex& v0 = vertices[indices[i]];
		Vertex& v1 = vertices[indices[i + 1]];
		Vertex& v2 = vertices[indices[i + 2]];

		const glm::vec3& pos0 = v0.Position;
		const glm::vec3& pos1 = v1.Position;
		const glm::vec3& pos2 = v2.Position;

		const glm::vec2& uv0 = v0.TexCoords;
		const glm::vec2& uv1 = v1.TexCoords;
		const glm::vec2& uv2 = v2.TexCoords;

		glm::vec3 edge1 = pos1 - pos0;
		glm::vec3 edge2 = pos2 - pos0;
		glm::vec2 deltaUV1 = uv1 - uv0;
		glm::vec2 deltaUV2 = uv2 - uv0;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
		
		glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
		glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

		v0.Tangent += tangent;
		v1.Tangent += tangent;
		v2.Tangent += tangent;
		v0.Bitangent += bitangent;
		v1.Bitangent += bitangent;
		v2.Bitangent += bitangent;
	}

	for (auto& vertex : vertices) {
		glm::vec3 N = glm::normalize(vertex.Normal);
		glm::vec3 T = vertex.Tangent;

		T = glm::normalize(T - N * glm::dot(N, T));

		glm::vec3 B = glm::cross(N, T);
		if (glm::dot(B, vertex.Bitangent) < 0.0f) {
			B = -B;
		}

		vertex.Tangent = T;
		vertex.Bitangent = glm::normalize(B);
	}
}

void Model::draw(Shader& shader)
{
	for (unsigned int i = 0; i < modelMeshes.size(); i++) {
		modelMeshes[i].Draw(shader);
	}
}

void Model::setTexture(const char* diffusePath, const char* specularPath, const char* normalPath)
{
	vector<shared_ptr<Texture>> newTextures;

	shared_ptr<Texture> diffuse = make_shared<Texture>();
	diffuse->loadFromFile(diffusePath);
	diffuse->type = "diffuseTexture";
	newTextures.push_back(diffuse);

	if (specularPath) {
		shared_ptr<Texture> specular = make_shared<Texture>();
		specular->loadFromFile(specularPath);
		specular->type = "specularTexture";
		newTextures.push_back(specular);
	}

	if (normalPath) {
		shared_ptr<Texture> normal = make_shared<Texture>();
		normal->loadFromFile(normalPath);
		normal->type = "normalTexture";
		newTextures.push_back(normal);
	}

	// Setze die neuen Texturen für alle Meshes
	for (unsigned int i = 0; i < modelMeshes.size(); i++) {
		modelMeshes[i].textures = newTextures;
	}

	cout << "Set custom textures for " << modelMeshes.size() << " meshes" << endl;
}


void Model::loadModel(string const& path, bool flipUVs)
{
	if (flipUVs) {
		scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace | //for tangents and bitangents
			aiProcess_JoinIdenticalVertices |
			aiProcess_FlipUVs
		);
	}
	else {
		scene = importer.ReadFile(
			path,
			aiProcess_Triangulate |
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace | //for tangents and bitangents
			aiProcess_JoinIdenticalVertices
		);
	}

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
		//if (mesh->mTangents && mesh->mBitangents)
		//{
		//	aiVector3D& tangent = mesh->mTangents[i];
		//	v.Tangent = glm::vec3(tangent.x, tangent.y, tangent.z);
		//	aiVector3D& bitangent = mesh->mBitangents[i];
		//	v.Bitangent = glm::vec3(bitangent.x, bitangent.y, bitangent.z);
		//}
		//else {
		//	v.Tangent = glm::vec3(0.0f);
		//	v.Bitangent = glm::vec3(0.0f);
		//}
		
		v.Tangent = glm::vec3(0.0f);
		v.Bitangent = glm::vec3(0.0f);

		//initialize bone data
		//for (int j = 0; j < MAX_BONE_INFLUENCE; j++)
		//{
		//	v.boneIDs[j] = 0;
		//	v.weights[j] = 0.0f;
		//}

		positions.push_back(v.Position);
		verts.push_back(v);
	}

	// bones
	processBones(verts, mesh);

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// compute tangents and bitangents
	computeTangentsBitangents(verts, indices);

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

void Model::processBones(vector<Vertex>& vertices, aiMesh* mesh)
{
	auto& tempBoneInfoMap = boneInfoMap;
	unsigned int& tempBoneCounter = boneCounter;

	for (unsigned int i = 0; i < mesh->mNumBones; ++i)
	{
		int boneID = -1;
		aiBone* bone = mesh->mBones[i];
		string boneName(bone->mName.C_Str());

		// if bone not present in map, add it
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = tempBoneCounter;
			newBoneInfo.offset = convertAiMatrixToGlm(bone->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = tempBoneCounter;
			tempBoneCounter++;

		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}

		assert(boneID != -1);

		auto weights = bone->mWeights;

		// assign weights to vertices
		for (unsigned int j = 0; j < bone->mNumWeights; ++j)
		{
			int vertexID = weights[j].mVertexId;
			float weightValue = weights[j].mWeight;
			assert(vertexID <= vertices.size());
			Vertex& vertex = vertices[vertexID];

			for (int k = 0; k < MAX_BONE_INFLUENCE; ++k)
			{
				if (vertex.boneIDs[k] < 0)
				{
					vertex.weights[k] = weightValue;
					vertex.boneIDs[k] = boneID;
					break;
				}
			}
		}
	}
}

void Model::loadMaterialTextures(aiMesh* mesh, std::vector<std::shared_ptr<Texture>>& meshTextures)
{

	if (!scene || !mesh) return;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (!material) return;

	aiTextureType types[4] = { aiTextureType_DIFFUSE, aiTextureType_SPECULAR, aiTextureType_NORMALS, aiTextureType_EMISSIVE }; //TODO add more?
	string typeNames[4] = { "diffuseTexture", "specularTexture", "normalTexture", "emissionTexture" };

	for (int i = 0; i < 4; i++)
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
		tex->loadFromFile("assets/models/Medieval tower/Medieval tower_mid_Col.jpg");
		tex->type = "diffuseTexture";

		meshTextures.push_back(tex);
	}
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
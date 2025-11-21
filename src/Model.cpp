#include "Model.h"

Model::Model(string const& path, bool gamma)
{
	loadModel(path);
}

void Model::loadModel(string const& path)
{
	Assimp::Importer importer;
	scene = importer.ReadFile(
		path, 
		aiProcess_Triangulate |
		aiProcess_GenNormals | //GenSmoothNormals 
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
		meshes.push_back(processMesh(mesh));
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

		positions.push_back(v.Position);
		verts.push_back(v);
	}

	// bones

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
	return Mesh(verts, indices, meshTextures);
}

void Model::loadMaterialTextures(aiMesh* mesh, std::vector<std::shared_ptr<Texture>>& meshTextures)
{

	if (!scene || !mesh) return;

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	if (!material) return;

	aiTextureType types[2] = { aiTextureType_DIFFUSE, aiTextureType_NORMALS }; //TODO add more?
	string typeNames[2] = { "diffuseTexture", "normalTexture" };

	for(int i = 0; i < 2; i++)
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
	if(material->GetTextureCount(aiTextureType_DIFFUSE) == 0)
	{
		cout << "No diffuse texture found for mesh. Using default texture" << endl;
		
		shared_ptr<Texture> tex = make_shared<Texture>();
		tex->loadFromFile("src/diffuse.jpg");
		tex->type = "diffuseTexture";

		meshTextures.push_back(tex);
	}
}

vector<shared_ptr<Texture>> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const string& typeName)
{
	vector<shared_ptr<Texture>> textures;

	unsigned int textureCount = mat->GetTextureCount(type);
	for (unsigned int i = 0; i < textureCount; i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// Build full file path: use model directory + relative path from Assimp
		string filePath = "src/backpack/" + string(str.C_Str());

		// Check if this texture was already loaded (by path)
		bool skip = false;
		for (unsigned int j = 0; j < loadedTexturePaths.size(); j++)
		{
			if (loadedTexturePaths[j] == filePath)
			{
				// reuse existing shared_ptr
				textures.push_back(loadedTextures[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// Not loaded yet: create and load a new Texture
			std::shared_ptr<Texture> tex = std::make_shared<Texture>();
			tex->loadFromFile(filePath.c_str());
			tex->type = typeName;

			// store in global model cache
			loadedTextures.push_back(tex);
			loadedTexturePaths.push_back(filePath);

			// add to this mesh
			textures.push_back(tex);

			std::cout << "Loaded new " << typeName
				<< " texture from " << filePath << std::endl;
		}
	}

	return textures;
}

void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}
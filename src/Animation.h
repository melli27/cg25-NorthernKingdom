#pragma once
#include "Utils/Utils.h"
#include <assimp/Importer.hpp>
#include <Model/Model.h>
#include <Bone.h>

struct AssimpNodeData
{
	glm::mat4 transformation;
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, Model* model);

	~Animation(){}

	Bone* FindBone(const std::string& name);
	
	float GetTicksPerSecond() const { return m_TicksPerSecond; }
	float GetDuration() const { return m_Duration; }
	const AssimpNodeData& GetRootNode() { return m_RootNode; }
	const std::map<std::string, BoneInfo>& GetBoneIDMap() { return m_BoneInfoMap; }

private:

	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;

	void ReadMissingBones(const aiAnimation* animation, Model& model);

	void ReadHierarchyData(AssimpNodeData& dest, const aiNode* src);

	glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from);

	glm::vec3 GetGLMVec(const aiVector3D& vec)
	{
		return glm::vec3(vec.x, vec.y, vec.z);
	}

	glm::quat GetGLMQuat(const aiQuaternion& pOrientation)
	{
		return glm::quat(pOrientation.w, pOrientation.x, pOrientation.y, pOrientation.z);
	}
};
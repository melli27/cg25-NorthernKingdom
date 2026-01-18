#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

const int MAX_BONES = 200;
uniform mat4 boneMatrices[200];
uniform bool isAnimated;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out ivec4 oboneIDs;
//out mat3 TBN;

void main()
{
    vec4 vertexPos;

    if(isAnimated)
    {
        mat4 boneTransform = boneMatrices[boneIDs[0]] * weights[0];
        boneTransform += boneMatrices[boneIDs[1]] * weights[1];
        boneTransform += boneMatrices[boneIDs[2]] * weights[2];
        boneTransform += boneMatrices[boneIDs[3]] * weights[3];

        // Apply bone transformation to vertex position and normal
        vec4 skinnedPos = boneTransform * vec4(position, 1.0);
        vertexPos = modelMatrix * skinnedPos;
        mat3 normalMat = mat3(transpose(inverse(modelMatrix)));
        Normal = normalMat * mat3(boneTransform) * normal;    
    }
    else
    {
        vertexPos = modelMatrix * vec4(position, 1.0);
        Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    }

    FragPos = vec3(vertexPos);

    TexCoords = uv;
    oboneIDs = boneIDs;
    gl_Position = viewProjMatrix * vertexPos;
}
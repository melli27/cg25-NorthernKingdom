#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

uniform mat4 modelMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 boneMatrices[200];
uniform bool isAnimated;

void main() 
{
    vec4 pos = vec4(position, 1.0);

    if(isAnimated)
    {
        mat4 boneTransform = boneMatrices[boneIDs[0]] * weights[0];
        boneTransform += boneMatrices[boneIDs[1]] * weights[1];
        boneTransform += boneMatrices[boneIDs[2]] * weights[2];
        boneTransform += boneMatrices[boneIDs[3]] * weights[3];

        // Apply bone transformation to vertex position
        pos = boneTransform * pos;
    }

    gl_Position = lightSpaceMatrix * modelMatrix * pos;
}

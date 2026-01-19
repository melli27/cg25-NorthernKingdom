#version 430 core

layout (location = 0) in vec3 position;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 modelMatrix;
uniform mat4 boneMatrices[200];
uniform bool isAnimated;

out vec3 WorldPos;

void main()
{
    vec4 pos = vec4(position, 1.0);
    
    if(isAnimated)
    {
        mat4 boneTransform = boneMatrices[boneIDs[0]] * weights[0];
        boneTransform += boneMatrices[boneIDs[1]] * weights[1];
        boneTransform += boneMatrices[boneIDs[2]] * weights[2];
        boneTransform += boneMatrices[boneIDs[3]] * weights[3];
        
        pos = boneTransform * pos;
    }
    
    vec4 world = modelMatrix * pos;
    WorldPos = world.xyz;
    gl_Position = world;
}
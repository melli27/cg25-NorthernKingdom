#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weights;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 boneMatrices[200];
uniform bool isAnimated;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;
out vec3 Tangent;
out vec3 Bitangent;

void main() 
{
    vec4 worldPos;
    vec3 T;
    vec3 N;
    vec3 B;

    if(isAnimated)
    {
        mat4 boneTransform = boneMatrices[boneIDs[0]] * weights[0];
        boneTransform += boneMatrices[boneIDs[1]] * weights[1];
        boneTransform += boneMatrices[boneIDs[2]] * weights[2];
        boneTransform += boneMatrices[boneIDs[3]] * weights[3];

        // Apply bone transformation to vertex position and normal
        vec4 skinnedPos = boneTransform * vec4(position, 1.0);
        worldPos = modelMatrix * skinnedPos;
        mat3 normalMat = mat3(transpose(inverse(modelMatrix)));
        N = normalize(normalMat * mat3(boneTransform) * normal);
        T = normalize(mat3(modelMatrix) * mat3(boneTransform) * tangent);
        B = normalize(mat3(modelMatrix) * mat3(boneTransform) * bitangent);
    }
    else
    {
        worldPos = modelMatrix * vec4(position, 1.0);
        N = normalize(mat3(transpose(inverse(modelMatrix))) * normal);
        T = normalize(mat3(modelMatrix) * tangent);
        B = normalize(mat3(modelMatrix) * bitangent);
    }

    T = normalize(T - N * dot(N, T));
    vec3 Bfixed = cross(N, T);
    if (dot(Bfixed, B) < 0.0) {
        Bfixed = -Bfixed;
    }

    FragPos = vec3(worldPos);

    Normal = N;
    Tangent = T;
    Bitangent = normalize(Bfixed);

    TexCoords = uv;
    FragPosLightSpace = lightSpaceMatrix * worldPos;
    gl_Position = viewProjMatrix * worldPos;
}

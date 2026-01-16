#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4 weigths;

const int MAX_BONES = 100;
uniform mat4 boneMatrices[MAX_BONES];
uniform bool isAnimated;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
    vec4 vertexPos;

    if(isAnimated)
    {
        // Calculate Bone Transform for skinning
        mat4 boneTransform = mat4(0.0);
        for(int i = 0; i < 4; i++)
        {
            if(boneIDs[i] >= 0)  // Prüfe auf gültige IDs!
            {
                boneTransform += boneMatrices[boneIDs[i]] * weigths[i];
            }
        }
    
        // Wenn keine Bones zugeordnet sind, verwende Identity Matrix
        if(boneTransform == mat4(0.0))
        {
            boneTransform = mat4(1.0);
        }

        // Apply bone transformation to vertex position and normal
        vec4 vertexPos = (modelMatrix * boneTransform) * vec4(position, 1.0);
        Normal = mat3(transpose(inverse(modelMatrix * boneTransform))) * normal;
    }
    else
    {
        vertexPos = modelMatrix * vec4(position, 1.0);
        Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    }

    FragPos = vec3(vertexPos);

    vec3 T = normalize(vec3(modelMatrix * vec4(tangent, 0.0)));
    vec3 B = normalize(vec3(modelMatrix * vec4(bitangent, 0.0)));
    vec3 N = normalize(Normal - dot(Normal, T) * T - dot(Normal, B) * B); //TODO
    mat3 TBN = mat3(T, B, N);

    TexCoords = uv;

    gl_Position = viewProjMatrix * vertexPos;
}
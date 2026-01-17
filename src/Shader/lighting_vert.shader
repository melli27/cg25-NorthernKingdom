#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;
uniform mat4 lightSpaceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 FragPosLightSpace;

void main() 
{
    vec4 worldPos = modelMatrix * vec4(position, 1.0);
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(modelMatrix))) * normal;
    TexCoords = uv;
    FragPosLightSpace = lightSpaceMatrix * worldPos;
    gl_Position = viewProjMatrix * worldPos;
}

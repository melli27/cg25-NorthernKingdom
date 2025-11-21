#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 fragPos;
out vec2 TexCoords;

out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;

void main() 
{
	fragPos = vec3(modelMatrix * vec4(position, 1.0));
	TexCoords = uv;

    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 T = normalize(normalMatrix * tangent);
    vec3 N = normalize(normalMatrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    
    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightPos = TBN * lightPos;
    TangentViewPos  = TBN * viewPos;
    TangentFragPos  = TBN * fragPos;
        
	gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1.0);
}
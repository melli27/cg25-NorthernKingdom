#version 450 core

struct Material
{
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    sampler2D normalTexture;
    float shininess;
};

in vec3 FragPos;  
in vec2 TexCoords;
//in vec3 Normal;
flat in ivec4 oboneIDs;
//in mat3 TBN;

out vec4 FragColor;

//uniform bool normalMapping = true;
uniform Material material;

void main()
{
    FragColor = texture(material.diffuseTexture, TexCoords);
}




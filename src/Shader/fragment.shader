#version 450 core

in vec2 TexCoords;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D specularTexture;

out vec4 FragColor;

void main()
{

    FragColor = texture(diffuseTexture, TexCoords);
}
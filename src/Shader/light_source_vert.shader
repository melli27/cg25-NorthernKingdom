#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 modelMatrix;
uniform mat4 viewProjMatrix;

void main() 
{
	gl_Position = viewProjMatrix * modelMatrix * vec4(position, 1);
}
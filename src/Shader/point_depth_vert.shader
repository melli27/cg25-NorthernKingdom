#version 430 core
layout (location = 0) in vec3 position;

uniform mat4 modelMatrix;

out vec3 WorldPos;

void main()
{
    vec4 world = modelMatrix * vec4(position, 1.0);
    WorldPos = world.xyz;
    gl_Position = world;
}  
#version 450 core

layout(location = 0) in vec3 aPos; //position
layout(location = 1) in vec2 heightMap;

out vec2 HeightMapCoord;

void main() 
{
    gl_Position = vec4(aPos, 1.0);
    HeightMapCoord = heightMap;
}
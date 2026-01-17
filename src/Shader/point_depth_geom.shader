#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

in vec3 WorldPos[];

uniform mat4 shadowMatrices[6];

out vec3 FragWorldPos;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for (int i = 0; i < 3; ++i) // for each triangle's vertices
        {
            FragWorldPos = WorldPos[i];
            gl_Position = shadowMatrices[face] * vec4(WorldPos[i], 1.0);
            EmitVertex();
        }    
        EndPrimitive();
    }
} 
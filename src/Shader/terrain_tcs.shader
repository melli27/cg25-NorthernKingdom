#version 450 core

layout(vertices = 4) out;

uniform mat4 model;
uniform mat4 view;
uniform vec3 cameraPos;

uniform float minTessLevel = 4.0;
uniform float maxTessLevel = 64.0;
uniform float minDistance = 20.0;
uniform float maxDistance = 800.0;

in vec2 HeightMapCoord[];
out vec2 HeightMapCoordinates[];

void main()
{
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    HeightMapCoordinates[gl_InvocationID] = HeightMapCoord[gl_InvocationID];
    
    if (gl_InvocationID == 0)
    {
        // Transform vertex to eye space/world pos
        vec4 eyespacePos00 = view * model * gl_in[0].gl_Position;
        vec4 eyespacePos01 = view * model * gl_in[1].gl_Position;
        vec4 eyespacePos10 = view * model * gl_in[2].gl_Position;
        vec4 eyespacePos11 = view * model * gl_in[3].gl_Position;

        // Normaized distance to camera scaled [0,1]
        float dist00 = clamp((abs(eyespacePos00.z) - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float dist01 = clamp((abs(eyespacePos01.z) - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float dist10 = clamp((abs(eyespacePos10.z) - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float dist11 = clamp((abs(eyespacePos11.z) - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        
        // Intepolate edge tesselation levels based on distance (closser vertex)
        float t0 = mix(maxTessLevel, minTessLevel, min(dist10, dist00));
        float t1 = mix(maxTessLevel, minTessLevel, min(dist00, dist01));
        float t2 = mix(maxTessLevel, minTessLevel, min(dist01, dist11));
        float t3 = mix(maxTessLevel, minTessLevel, min(dist11, dist10));

        // Outer Levels
        gl_TessLevelOuter[0] = t0;
        gl_TessLevelOuter[1] = t1;
        gl_TessLevelOuter[2] = t2;
        gl_TessLevelOuter[3] = t3;
        
        // Inner Levels
        gl_TessLevelInner[0] = max(t1, t3);
        gl_TessLevelInner[1] = max(t0, t2);
    }
}
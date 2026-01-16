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
        // Berechne World-Space Positionen der 4 Patch-Eckpunkte
        vec4 worldPos00 = model * gl_in[0].gl_Position;
        vec4 worldPos01 = model * gl_in[1].gl_Position;
        vec4 worldPos10 = model * gl_in[2].gl_Position;
        vec4 worldPos11 = model * gl_in[3].gl_Position;
        
        // Distanz von jedem Eckpunkt zur Kamera
        float distance00 = distance(worldPos00.xyz, cameraPos);
        float distance01 = distance(worldPos01.xyz, cameraPos);
        float distance10 = distance(worldPos10.xyz, cameraPos);
        float distance11 = distance(worldPos11.xyz, cameraPos);
        
        // Normalisiere Distanzen auf [0,1] für mix
        float t00 = clamp((distance00 - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float t01 = clamp((distance01 - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float t10 = clamp((distance10 - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float t11 = clamp((distance11 - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        
        // Berechne Tessellation-Level für jede Kante
        // Jede Kante nutzt die Eckpunkte die sie verbindet
        //float tessLevel0 = mix(maxTessLevel, minTessLevel, min(t10, t00)); // linke Kante (v2-v0)
        //float tessLevel1 = mix(maxTessLevel, minTessLevel, min(t00, t01)); // untere Kante (v0-v1)
        //float tessLevel2 = mix(maxTessLevel, minTessLevel, min(t01, t11)); // rechte Kante (v1-v3)
        //float tessLevel3 = mix(maxTessLevel, minTessLevel, min(t11, t10)); // obere Kante (v3-v2)
        
        vec3 center = (worldPos00.xyz + worldPos01.xyz + worldPos10.xyz + worldPos11.xyz) * 0.25;
        float d = distance(center, cameraPos);
        float t = clamp((d - minDistance) / (maxDistance - minDistance), 0.0, 1.0);
        float tess = mix(maxTessLevel, minTessLevel, t);

        // Setze Outer-Levels
        gl_TessLevelOuter[0] = tess;
        gl_TessLevelOuter[1] = tess;
        gl_TessLevelOuter[2] = tess;
        gl_TessLevelOuter[3] = tess;
        
        // Inner-Levels: Maximum der gegenüberliegenden Kanten
        gl_TessLevelInner[0] = tess;
        gl_TessLevelInner[1] = tess;
    }
}
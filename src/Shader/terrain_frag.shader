#version 450 core

in float Height;
in vec4 textPos;
in vec2 TexCoord;

// Shadow
in vec4 FragPosLightSpace;
uniform sampler2D shadowMap;

// Rock
uniform sampler2D rockDiffuse;
uniform sampler2D rockNormal;

// Grass
uniform sampler2D grassDiffuse;
uniform sampler2D grassNormal;

// Snow
uniform sampler2D snowDiffuse;
uniform sampler2D snowNormal;

uniform vec3 lightDir;
uniform vec3 viewPos;

out vec4 FragColor;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    if(projCoords.z > 1.0)
        return 0.0;
    
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    
    float bias = 0.005;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    
    return shadow;
}

void computeWeights(float height, out float wGrass, out float wRock, out float wSnow)
{
    // Grass at bottom
    float g = 1.0 - smoothstep(0.22, 0.29, height); // 0.12-0.19 Uebergangszone
    
    // Snow at top
    float s = smoothstep(0.50, 0.60, height);
    
    // Rock in the middle
    float r = 1.0 - g - s;
    
    float sum = g + r + s + 0.0001;
    wGrass = g / sum;
    wRock  = r / sum;
    wSnow  = s / sum;
}

void main()
{
    float wGrass, wRock, wSnow;

    float h0 = clamp((Height + 100.0) / 100.0, 0.0, 1.0);
    computeWeights(h0, wGrass, wRock, wSnow);

    // Tiling factor
    vec2 uv = TexCoord * 20.0;

    vec3 colGrass = texture(grassDiffuse, uv).rgb;
    vec3 colRock  = texture(rockDiffuse,  uv).rgb;
    vec3 colSnow  = texture(snowDiffuse,  uv).rgb;

    vec3 color = colGrass * wGrass + colRock * wRock + colSnow * wSnow;

    float shadow = ShadowCalculation(FragPosLightSpace);
    vec3 finalColor = color * (1.0 - shadow * 0.3); // Darken color in shadow

    FragColor = vec4(finalColor, 1.0);
}
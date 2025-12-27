#version 450 core

in float Height;
in vec4 textPos;
in vec2 TexCoord;

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

void computeWeights(float height, out float wGrass, out float wRock, out float wSnow)
{
    // Gras on bottom
    float g = 1.0 - smoothstep(0.12, 0.19, height); // 0.12–0.1ß Übergangszone
    
    // Snow on top
    float s = smoothstep(0.20, 0.35, height);
    
    // Rock in between
    float r = 1.0 - g - s;
    
    float sum = g + r + s + 0.0001;
    wGrass = g / sum;
    wRock  = r / sum;
    wSnow  = s / sum;
}

void main()
{
    float wGrass, wRock, wSnow;

    float h0 = clamp((Height + 100.0) / 200.0, 0.0, 1.0);
    computeWeights(h0, wGrass, wRock, wSnow);

    // Tiling factor
    vec2 uv = TexCoord * 20.0;

    vec3 colGrass = texture(grassDiffuse, uv).rgb;
    vec3 colRock  = texture(rockDiffuse,  uv).rgb;
    vec3 colSnow  = texture(snowDiffuse,  uv).rgb;

    vec3 color = colGrass * wGrass + colRock * wRock + colSnow * wSnow;

    FragColor = vec4(color, 1.0);
}

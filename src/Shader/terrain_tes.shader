#version 450 core
layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D heightMap;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec2 HeightMapCoordinates[];

out float Height;
//out vec4 textPos;

out vec2 TexCoord; //

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // Bilinear interpolation of UVs from the 4 control point UVs
    vec2 uv0 = mix(HeightMapCoordinates[0], HeightMapCoordinates[1], u);
    vec2 uv1 = mix(HeightMapCoordinates[2], HeightMapCoordinates[3], u);
    vec2 uv = mix(uv0, uv1, v);
    Height = texture(heightMap, uv).r * 200.0 - 100.0; // * scale - offset  64.0-16.0  Displacementscale

    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p10 = gl_in[2].gl_Position;
    vec4 p11 = gl_in[3].gl_Position;

    vec4 uVec = p01 - p00;
    vec4 vVec = p10 - p00;
    vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

    vec4 p0 = (p01 - p00) * u + p00;
    vec4 p1 = (p11 - p10) * u + p10;
    vec4 p = (p1 - p0) * v + p0 + normal * Height;
    //vec4 p = (p1 - p0) * v + p0;
    //p += normal * Height;

    //textPos = p;
    TexCoord = uv;
    gl_Position = projection * view * model * p;
}
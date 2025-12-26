#version 450 core

in float Height;
in vec4 textPos;
in vec2 TexCoord;

uniform sampler2D surfaceTexture;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(surfaceTexture, TexCoord);

    // use height for shading effect
    float h = (Height + 150)/200.0f;
    
    FragColor = texColor * vec4(h, h, h, 1.0);
}
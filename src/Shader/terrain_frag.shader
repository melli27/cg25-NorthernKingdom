#version 450 core

in float Height;
in vec4 textPos;
in vec2 TexCoord; // new

//uniform sampler2D surfaceTexture;

out vec4 FragColor;

void main()
{
    //vec4 texColor = texture(surfaceTexture, TexCoord);
    //FragColor = texColor;

    // use height for shading effect
    float h = (Height + 16)/64.0f;
    FragColor = vec4(h, h, h, 1.0);
    // FragColor = texColor * vec4(h, h, h, 1.0);
}
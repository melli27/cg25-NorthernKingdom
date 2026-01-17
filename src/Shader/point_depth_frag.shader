#version 430 core
in vec3 FragWorldPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(FragWorldPos - lightPos);
  
    lightDistance = lightDistance / farPlane;

    gl_FragDepth = lightDistance;
}  
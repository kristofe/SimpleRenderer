#version 150
in vec2 inuv;
in vec3 incolor;
out vec4 finalColor;

uniform float time;

void main() 
{
  finalColor = vec4(incolor,1);
}

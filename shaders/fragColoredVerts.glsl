#version 150
in vec2 inuv;
in vec4 incolor;
in vec3 innormal;
in vec4 intangent;
out vec4 finalColor;

uniform float time;
uniform sampler2D texture0;
void main() 
{
  //finalColor = incolor;
  //finalColor = vec4(innormal,1);
  finalColor = intangent;
}

#version 150

in vec3 vNormal;
in vec2 vUV;
in vec4 vColor;
in vec4 vTangent;

uniform sampler2D texture0;

out vec4 finalColor;

void main() 
{
  vec4 t = texture(texture0, vUV);
  finalColor = t;
}

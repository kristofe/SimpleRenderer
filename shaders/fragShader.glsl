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
  vec4 t = texture(texture0, inuv);
  //finalColor = vec4(inuv,1,1);
  float d = clamp(dot(innormal, normalize(vec3(1,1,1))),0, 1);
  float a = 0.35f;
  finalColor = (d*(1.0f-a)+ a) * t;
  //finalColor = t;
  //finalColor = vec4(1,0,0,1);
}

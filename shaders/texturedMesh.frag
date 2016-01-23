#version 150

in vec3 vNormal;
in vec2 vUV;
in vec4 vColor;
in vec4 vTangent;

uniform sampler2D texture0;

out vec4 finalColor;

void main() 
{
  //vec3 n = vNormal;
  //vec2 uv = vec2(gl_FragCoord.x/512.0, gl_FragCoord.y/512.0);
  vec4 t = texture(texture0, vUV);
  //vec4 t = texture(texture0, uv);
  finalColor = t;//vColor * t;
  //finalColor = vec4(uv.xy,0,1);
}

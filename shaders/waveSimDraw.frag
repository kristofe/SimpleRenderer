#version 150
uniform sampler2D uRefraction;
uniform samplerCube uCubeReflect;
uniform vec3 uWorldSpaceCameraPos;

in vec2 vUV;
in vec3 vNormal;
in vec3 vTangent;
in vec4 vVertColor;
in vec3 vReflection;
//in vec3 vRefraction;
in vec2 vNormScreenCoord;
in vec3 vEye;

out vec4 color;

float Fresnel(float NdotL, float fresnelBias, float fresnelPow)
{
  float facing = (1.0 - NdotL);
  return max(fresnelBias +
             (1.0 - fresnelBias) * pow(facing, fresnelPow), 0.0);
}

void main()
{
  float d = clamp(dot(vNormal, normalize(vec3(1,1,1))),0, 1);
  vec4 diffuse = vec4(d,d,d,1);
  color = vec4(vNormal,1);

  vec4 reflect_color =texture(uCubeReflect, vReflection);
  vec4 refract_color =texture(uRefraction, vNormal.xz* 0.4 + vNormScreenCoord);

  float NdotL = max(dot(vEye, vReflection), 0);
  float fresnel = Fresnel(NdotL, 0.2, 1.5);


  //color = mix(refract_color, reflect_color, 0.4);
  fresnel = mix(0.1, 0.8, fresnel);
  color = refract_color * (1.0-fresnel) + reflect_color * (fresnel);
  //color = mix(refract_color, reflect_color, fresnel);
}

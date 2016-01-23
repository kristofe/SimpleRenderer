#version 150
uniform sampler2D uPaintTexture;
uniform samplerCube uCubeReflect;
uniform sampler2D uNormalMap;
uniform vec3 uWorldSpaceCameraPos;
uniform float uDrawWhite;

in vec2 vUV;
in vec3 vNormal;
in vec3 vTangent;
in vec4 vVertColor;
in vec3 vReflection;
//in vec3 vRefraction;
in vec2 vNormScreenCoord;
in vec3 vEye;

out vec4 color;

//FIXME: z any axis seemed to be swapped
void main()
{
  const vec3 light = normalize(vec3(0.1,0.4,-2.0));
  const vec3 light2 = normalize(vec3(-0.4,0.8,0.3));

  vec4 baseColor = texture(uPaintTexture, vUV);
  float alpha = baseColor.a;//0.21*baseColor.r + 0.72*baseColor.g + 0.07*baseColor.b;
  float bias = smoothstep(0.4, 0, alpha);
  vec3 srcNormal = normalize(texture(uNormalMap, vUV*1.75).xyz * 2.0 - 1.0);
  //vec3 normal = srcNormal* smoothstep(0.008, 0, alpha) + vNormal;
  vec3 normal = srcNormal* smoothstep(0.001, 0, 0.1 * alpha) + vNormal;

  normal = normalize(normal);

  float d = clamp(dot(normal, light),0, 1);
  float sp = pow(max(0.0, dot(reflect(-light, vNormal), -vEye)),16.0);
  d += clamp(dot(normal, light2),0, 1);
  sp += pow(max(0.0, dot(reflect(-light2, vNormal), -vEye)),16.0);
  
  vec4 diffuse = vec4(d,d,d,1);
  vec4 specular = vec4(sp,sp,sp,1);

  vec4 newColor = baseColor*0.3 + baseColor*diffuse*0.6 + 0.3*bias*specular;


  //Lumosity formula to convert color to greyscale
  color =  mix(newColor, vec4(1),uDrawWhite);
  color.a = alpha;
  //color = mix(refract_color, reflect_color, fresnel);
}

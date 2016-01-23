#version 150
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;

uniform sampler2D uWaveTexture;
uniform sampler2D uRefraction;
uniform vec2 uFBOSize;
uniform float uWaveScale;
uniform mat4 uModelTransform;
uniform mat4 uModelViewTransform;
uniform mat4 uMVP;
uniform vec3 uWorldSpaceCameraPos;

out vec2 vUV;
out vec3 vNormal;
out vec3 vTangent;
out vec4 vVertColor;
out vec3 vReflection;
//out vec3 vRefraction;
out vec2 vNormScreenCoord;
out vec3 vEye;


void main() {
  vec2 step =vec2(1.0f/uFBOSize);
  const ivec3 off1 = ivec3(1,0,1);
  const ivec3 off2 = ivec3(2,0,2);
  vec2 dxo = vec2(step.x,0);
  vec2 dyo = vec2(0,step.y);
  vec4 origin =  texture(uWaveTexture,uv);
  
  //Commented out averaging code because the larger input radius smoothed 
  //everything
  /*
  origin +=  textureOffset(uWaveTexture,uv, off1.xy);
  origin +=  textureOffset(uWaveTexture,uv, off1.zy);
  origin +=  textureOffset(uWaveTexture,uv, -off1.xy);
  origin +=  textureOffset(uWaveTexture,uv, -off1.zy);
  origin *= 0.2f;
  */
  vec4 right = texture(uWaveTexture, uv + dxo);
  vec4 up = texture(uWaveTexture, uv + dyo);

  //up is Z!
  vec3 o = vec3(0,0,origin.x);
  vec3 u = vec3(0,step.y,up.x);
  vec3 r = vec3(step.x,0,right.x);

  vec3 xDir = r - o;
  vec3 yDir = u - o;
  
  vNormal = cross(xDir, yDir);

  //Using cheaper normal calculation above (1/2 the texture samples of code 
  //below)
  /*
  vec2 size = vec2(2.0*step.x,0.0);
  const ivec3 off = ivec3(-1,0,1);

  vec4 wave = texture(uWaveTexture, uv);
  float s11 = wave.x;
  float s01 = textureOffset(uWaveTexture, uv, off.xy).x;
  float s21 = textureOffset(uWaveTexture, uv, off.zy).x;
  float s10 = textureOffset(uWaveTexture, uv, off.yx).x;
  float s12 = textureOffset(uWaveTexture, uv, off.yz).x;
  vec3 va = normalize(vec3(size.xy,s21-s01));
  vec3 vb = normalize(vec3(size.yx,s12-s10));
  vec4 bump = vec4( cross(va,vb), s11 );
  vNormal = bump.xyz;
  */

  //Up is Z!
  vec4 p = vec4(position, 1.0);
  p.z += origin.x * 10.0 * uWaveScale;

  vec3 worldPos = vec3(uModelTransform * p);
  vNormal = mat3(uModelTransform) * vNormal;
  vNormal = normalize(vNormal);
  gl_Position = uMVP * p;

  vec3 view = normalize(worldPos.xyz - uWorldSpaceCameraPos);
  vReflection = reflect(view, vNormal);
  //vRefraction = refract(view, vNormal, 1.0/1.33);
  vVertColor = color;
  vUV = uv.xy;

  vec4 perspPos = uMVP * p;
  vNormScreenCoord = (perspPos / perspPos.w).xy;
  vNormScreenCoord = vNormScreenCoord * 0.5 + 0.5;
  vEye = normalize(uWorldSpaceCameraPos);
}


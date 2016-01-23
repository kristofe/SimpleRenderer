#version 150

in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;


uniform mat4 mtxModel;
uniform mat4 mtxNormal;
uniform mat4 mtxCameraInverse;
uniform mat4 mtxMVP;

uniform float time;
uniform sampler2D texture0;

out vec3 vNormal;
out vec2 vUV;
out vec4 vColor;
out vec4 vTangent;


void main() {
  gl_Position = mtxMVP * vec4(position, 1);
  vNormal = mat3(mtxNormal) * normal;
  vUV = uv;
  vColor = color;
  vTangent = tangent;
}

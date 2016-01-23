#version 150
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;


uniform mat4 ModelViewProjection;
uniform mat4 ModelView;
uniform float time;
out vec4 incolor;
out vec2 inuv;
out vec3 innormal;
out vec4 intangent;


void main() {
  gl_Position = vec4(position, 1);
  innormal = normal;
  intangent = tangent;
  inuv = uv;
  incolor =color;
}
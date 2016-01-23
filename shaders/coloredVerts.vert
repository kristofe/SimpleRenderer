#version 150
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;

uniform mat4 ModelViewProjection;
uniform float time;
out vec3 incolor;
out vec2 inuv;


void main() {
  gl_Position = ModelViewProjection * vec4(position, 1);
  //gl_Position = vec4(position, 1);
  incolor = color;
}

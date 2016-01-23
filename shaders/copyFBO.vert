#version 150

in vec3 position;
in vec2 uv;
uniform sampler2D texture0;

out vec2 vUV;


void main() {
  gl_Position = vec4(position, 1);
  vUV = uv;
}

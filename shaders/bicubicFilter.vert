#version 410
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;


out vec2 vUV;

void main(void) {
  vUV = uv.xy;
  gl_Position = vec4(position,1.0);
}

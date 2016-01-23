                      #version 150
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;

out vec2 vUV;
out vec3 vNormal;
out vec3 vTangent;
out vec4 vVertColor;


void main() {
  //gl_Position = uPMatrix * uMVMatrix * vec4(position, 1.0);
  gl_Position = vec4(position, 1.0);
  vVertColor = color;
  vUV = uv.xy;
}


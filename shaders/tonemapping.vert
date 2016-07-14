#version 410
in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;

out vec2 vUV;
uniform sampler2D texture0;

 void main()
 {
     gl_Position = vec4(position, 1.0);
     vUV = uv;
 }

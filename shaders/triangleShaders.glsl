-- VS
#ifdef GL_ES
precision highp float;
#endif


//in vec3 vert;
//in vec3 color;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 color;

uniform mat4 ModelViewProjection;
uniform float time;
out vec4 incolor;
out vec3 inuv;


void main() {
  //gl_Position = ModelViewProjection * vec4(vert, 1);
  gl_Position = vec4(vert, 1);
  inuv = vec3(color.xy,sin(time)*0.5 + 0.5);
  incolor = vec4(color,1.0);
}

-- FS
#ifdef GL_ES
precision highp float;
#endif
in vec3 inuv;
in vec4 incolor;
out vec4 finalColor;

void main() {

  finalColor = incolor;
}

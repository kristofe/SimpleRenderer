#version 410
#extension GL_ARB_explicit_attrib_location : enable

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 tangent;
layout(location=4) in vec4 color;

uniform mat4 mvp;
out vec3 outPos;
out vec4 outColor;

void main(void)
{

	gl_Position=vec4(position,1);//ftransform();
	outPos = position;

	outColor=color;

}

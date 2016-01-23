#version 150
#extension GL_ARB_explicit_attrib_location : enable

layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 uv;
layout(location=3) in vec4 tangent;
layout(location=4) in vec4 color;

uniform mat4 mvp;


//Vertices position for fragment shader
out vec4 inPosition;
out vec4 inColor;

void main(void)
{
	inPosition = vec4(position,1);
	gl_Position = mvp * vec4(position,1);

	inColor = color;

}

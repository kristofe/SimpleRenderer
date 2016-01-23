#version 150

in vec4 inPosition;
in vec4 inColor;
out vec4 color;

uniform vec3 dataStep;
uniform sampler3D dataFieldTex;



void main(void) {
	//gl_FragColor=gl_Color;
	color=vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

#version 150

uniform mat4 vw_mat;

in vec2 position;
in vec4 color;

out vec4 fColor;

void main(void) {	
	fColor = color;
	gl_Position = vw_mat * vec4(position, 0.0, 1.0);
}

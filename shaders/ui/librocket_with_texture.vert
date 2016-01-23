#version 150

uniform mat4 vw_mat;

in vec2 position;
in vec4 color;
in vec2	uv;

out vec4 f_color;
smooth out vec2 f_texture;

void main(void) {	
	f_color = color;
	f_texture = uv;
	gl_Position = vw_mat * vec4(position, 0.0, 1.0);
}

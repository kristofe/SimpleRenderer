#version 150

in vec3 position;
out vec4 vPosition;
uniform mat4 ModelviewProjection;

void main()
{
    vec4 Position = vec4(position,1);
    gl_Position = ModelviewProjection * Position;
    vPosition = Position;
}





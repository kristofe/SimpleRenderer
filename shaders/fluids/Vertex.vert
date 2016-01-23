#version 150
#extension GL_ARB_explicit_attrib_location : enable

in vec3 position;
in vec3 normal;
in vec2 uv;
in vec4 tangent;
in vec4 color;

void main()
{
    gl_Position = vec4(position,1);
}


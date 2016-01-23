#version 150


out vec4 FragColor;
uniform sampler2D Sampler;
uniform vec3 FillColor;
uniform vec2 Scale;

void main()
{
    float L = texture(Sampler, gl_FragCoord.xy * Scale).r;
    FragColor = vec4(FillColor, L);
}

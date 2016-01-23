#version 150

out vec4 FragColor;

uniform sampler3D VelocityTexture;
uniform sampler3D SourceTexture;
uniform sampler3D Obstacles;

uniform vec3 InverseSize;
uniform float TimeStep;
uniform float Dissipation;

in float gLayer;

void main()
{
    vec3 fragCoord = vec3(gl_FragCoord.xy, gLayer);
    float solid = texture(Obstacles, InverseSize * fragCoord).x;
    if (solid > 0) {
        FragColor = vec4(0);
        return;
    }

    vec3 u = texture(VelocityTexture, InverseSize * fragCoord).xyz;

    vec3 coord = InverseSize * (fragCoord - TimeStep * u);
    FragColor = Dissipation * texture(SourceTexture, coord);
}


#version 150

out vec2 FragColor;
uniform sampler2D Velocity;
uniform sampler2D Temperature;
uniform sampler2D Density;
uniform float AmbientTemperature;
uniform float TimeStep;
uniform float Sigma;
uniform float Kappa;

void main()
{
    ivec2 TC = ivec2(gl_FragCoord.xy);
    float T = texelFetch(Temperature, TC, 0).r;
    vec2 V = texelFetch(Velocity, TC, 0).xy;

    FragColor = V;

    if (T > AmbientTemperature) {
        float D = texelFetch(Density, TC, 0).x;
        FragColor += (TimeStep * (T - AmbientTemperature) * Sigma - D * Kappa ) * vec2(0, 1);
    }
}


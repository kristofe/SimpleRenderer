#version 410 core
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_ambient1;
    sampler2D texture_emissive1;
    sampler2D texture_height1;
    sampler2D texture_normal1;
    sampler2D texture_shininess1;
    sampler2D texture_opacity1;
    sampler2D texture_displacement1;
    sampler2D texture_lightmap1;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
    float shininessStrength;
    float refraction;
}; 
/* Note: because we now use a material struct you want to change your
shader class to bind all the textures using material.texture_diffuseN instead of
texture_diffuseN. */

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

in vec3 fragPosition;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

// Function prototypes
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    vec3 result;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 norm = normalize(Normal);
    
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], material, norm, fragPosition, viewDir);

    result += vec3(texture(material.texture_emissive1, TexCoords)) * material.emissive;

    color = vec4(result, 1.0f);
    //color =texture(material.texture_diffuse1, TexCoords);
}


// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, Material mat, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    //TODO:FIX SPEC FORMULA
    float spec = pow(max(dot(viewDir, reflectDir), mat.shininessStrength), mat.shininess);
    spec = max(spec,0.0);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient = light.ambient * vec3(texture(mat.texture_ambient1, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(mat.texture_diffuse1, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(mat.texture_specular1, TexCoords));
    ambient *= mat.ambient;
    diffuse *= mat.diffuse;
    specular *= mat.specular;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return specular + ambient + diffuse;
}
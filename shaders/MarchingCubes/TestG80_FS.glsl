#version 410

in vec3 localPos;
in vec4  outColor;
out vec4 color;

uniform vec3 dataStep;
uniform sampler3D dataFieldTex;

const vec3 diffuseMaterial = vec3(0.7, 0.7, 0.7);
const vec3 specularMaterial = vec3(0.99, 0.99, 0.99);
const vec3 ambiantMaterial = vec3(0.1, 0.1, 0.1);

const vec4 lightPos = vec4(5.0f, 5.0f, 5.0f, 1.0f);

void main(void)
{

#if 1
    vec3 grad = vec3(
        texture(dataFieldTex, (localPos.xyz+vec3(dataStep.x, 0, 0)+1.0f)/2.0f).r - texture(dataFieldTex, (localPos.xyz+vec3(-dataStep.x, 0, 0)+1.0f)/2.0f).r, 
        texture(dataFieldTex, (localPos.xyz+vec3(0, dataStep.y, 0)+1.0f)/2.0f).r - texture(dataFieldTex, (localPos.xyz+vec3(0, -dataStep.y, 0)+1.0f)/2.0f).r, 
        texture(dataFieldTex, (localPos.xyz+vec3(0,0,dataStep.z)+1.0f)/2.0f).r - texture(dataFieldTex, (localPos.xyz+vec3(0,0,-dataStep.z)+1.0f)/2.0f).r);
    

    
    vec3 lightVec=normalize(lightPos.xyz-localPos.xyz);
    
    vec3 normalVec = normalize(grad);

    vec3 incolor=outColor.rgb*0.5+abs(normalVec)*0.5;

    // calculate half angle vector
    vec3 eyeVec = vec3(0.0, 0.0, 1.0);
    vec3 halfVec = normalize(lightVec + eyeVec);
    
    // calculate diffuse component
    vec3 diffuse = vec3(abs(dot(normalVec, lightVec))) * incolor*diffuseMaterial;

    // calculate specular component
    vec3 specular = vec3(abs(dot(normalVec, halfVec)));
    //specular = pow(specular.x, 32.0) * specularMaterial;
    specular = pow(specular.x, 2.0) * specularMaterial;
    
    // combine diffuse and specular contributions and output final vertex color
    color.rgb =incolor.rgb*ambiantMaterial + diffuse + specular;
    color.a = 1.0;
    color.rgb = normalVec;
#else

    //outColor = color;
    outColor =vec4(1.0f, 0.0f, 0.0f, 1.0f);
#endif

    
}

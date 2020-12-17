#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position; //No espaço da câmera
};

uniform Light light;
uniform Material material;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;

uniform sampler2D sampler;

out vec3 finalColor;

void main()
{
    vec3 ambient = material.ambient * texture(sampler, fragUV).rgb;
    vec3 diffuse = vec3(0.0, 0.0, 0.0);
    vec3 specular = vec3(0.0, 0.0, 0.0);

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(light.position - fragPos);

    float iDif = dot(L, N);

    if (iDif > 0) {
        diffuse = iDif * material.diffuse * texture(sampler, fragUV).rgb;

        vec3 V = normalize(-fragPos); //V = eye - fragPos, mas, como estamos no espaço de view, eye = (0,0,0)
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N, H), 0.0), material.shininess);
        specular = iSpec * material.specular;
    }

    finalColor = ambient + diffuse + specular;

    finalColor = vec3(1.0f);
}

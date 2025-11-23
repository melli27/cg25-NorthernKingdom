#version 450 core

struct Material
{
    sampler2D diffuseTexture;
    sampler2D specularTexture;    
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    vec3 color;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;

vec3 calculateAmbientReflection()
{
    return dirLight.ambient * vec3(texture(material.diffuseTexture, TexCoords));
}

vec3 calculateDiffuseReflection(vec3 normal, vec3 lightDir)
{
    return dirLight.diffuse * vec3(texture(material.diffuseTexture, TexCoords)) * dirLight.color * max(dot(normal, lightDir), 0.0);
}

vec3 calculateSpecularReflection(vec3 normal, vec3 lightDir, vec3 viewDir)
{
    vec3 halfwayVector = normalize(lightDir + viewDir);
    return dirLight.specular * vec3(texture(material.specularTexture, TexCoords)) * dirLight.color * pow(max(dot(normal, halfwayVector), 0.0), material.shininess);
}

vec3 calculateDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);

    vec3 ambient = calculateAmbientReflection();
    vec3 diffuse = calculateDiffuseReflection(normal, lightDir);
    vec3 specular = calculateSpecularReflection(normal, lightDir, viewDir);

    return ambient + diffuse + specular;
}

vec3 calculatePointLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - FragPos);

    vec3 ambient = calculateAmbientReflection();
    vec3 diffuse = calculateDiffuseReflection(normal, lightDir);
    vec3 specular = calculateSpecularReflection(normal, lightDir, viewDir);

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.attenuation.x + pointLight.attenuation.y * distance + pointLight.attenuation.z * pow(distance, 2.0));

    
    return (ambient + diffuse + specular) * attenuation;
}


void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 color = calculateDirLight(normal, viewDir); // Blinn-Phong for directional light 
    color += calculatePointLight(normal, viewDir); // Blinn-Phong for point light
    FragColor = vec4(color, 1.0);
    //FragColor = texture(material.diffuseTexture, TexCoords);
}




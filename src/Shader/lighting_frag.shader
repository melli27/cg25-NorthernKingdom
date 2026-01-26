#version 450 core

struct Material
{
    sampler2D diffuseTexture;
    sampler2D specularTexture;
    sampler2D emissionTexture;
    sampler2D normalTexture;
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
in vec4 FragPosLightSpace;
in vec3 Tangent;
in vec3 Bitangent;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pointLight;
uniform sampler2D shadowMap;
uniform samplerCube pointShadowMap;
uniform float farPlane;
uniform int pointLightOn;
uniform int hasEmission;
uniform int normalMappingOn;
uniform int hasNormalMap;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float calculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0)
        return 0.0;
    
    float bias = max(0.0025 * (1.0 - dot(normal, lightDir)), 0.0005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            float currentDepth = projCoords.z;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}  

float calculatePointShadow(vec3 fragPos, vec3 lightPos, vec3 normal, vec3 lightDir)
{   
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = max(0.0025 * (1.0 - dot(normal, lightDir)), 0.0005);
    int samples  = 20;
    float diskRadius = (1.0 + (length(viewPos - fragPos) / farPlane)) / 25.0;  

    for (int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= farPlane;
        shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 calculateAmbientReflection(vec3 lightAmbient)
{
    return lightAmbient * vec3(texture(material.diffuseTexture, TexCoords));
}

vec3 calculateDiffuseReflection(vec3 lightDiffuse, vec3 lightColor, vec3 normal, vec3 lightDir)
{
    return lightDiffuse * vec3(texture(material.diffuseTexture, TexCoords)) * lightColor * max(dot(normal, lightDir), 0.0);
}

vec3 calculateSpecularReflection(vec3 lightSpecular, vec3 lightColor, vec3 normal, vec3 lightDir, vec3 viewDir)
{
    vec3 halfwayVector = normalize(lightDir + viewDir);
    return lightSpecular * vec3(texture(material.specularTexture, TexCoords)) * lightColor * pow(max(dot(normal, halfwayVector), 0.0), material.shininess);
}

vec3 calculateDirLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-dirLight.direction);

    vec3 ambient = calculateAmbientReflection(dirLight.ambient);
    vec3 diffuse = calculateDiffuseReflection(dirLight.diffuse, dirLight.color, normal, lightDir);
    vec3 specular = calculateSpecularReflection(dirLight.specular, dirLight.color, normal, lightDir, viewDir);

    float shadow = calculateShadow(FragPosLightSpace, normal, lightDir);

    return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 calculatePointLight(vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - FragPos);

    vec3 ambient = calculateAmbientReflection(pointLight.ambient);
    vec3 diffuse = calculateDiffuseReflection(pointLight.diffuse, pointLight.color, normal, lightDir);
    vec3 specular = calculateSpecularReflection(pointLight.specular, pointLight.color, normal, lightDir, viewDir);

    float distance = length(pointLight.position - FragPos);
    float attenuation = 1.0 / (pointLight.attenuation.x + pointLight.attenuation.y * distance + pointLight.attenuation.z * pow(distance, 2.0));

    float shadow = calculatePointShadow(FragPos, pointLight.position, normal, lightDir);    

    return (ambient + (1.0 - shadow) * (diffuse + specular)) * attenuation;
}


void main()
{
    vec3 normal = normalize(Normal);

    if (normalMappingOn == 1 && hasNormalMap == 1) {
        normal = texture(material.normalTexture, TexCoords).rgb;
        normal = normal * 2.0 - 1.0;
        mat3 TBN = mat3(normalize(Tangent), normalize(Bitangent), normalize(Normal));
        normal = normalize(TBN * normal);
    }

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 color = calculateDirLight(normal, viewDir); // Blinn-Phong for directional light

    if (pointLightOn == 1) {
        color += calculatePointLight(normal, viewDir); // Blinn-Phong for point light
        if (hasEmission == 1) {
            float emissionStrength = 6.0f;
            color += vec3(texture(material.emissionTexture, TexCoords)) * emissionStrength;
        }
    }

    //vec3 color = calculatePointLight(normal, viewDir);
    FragColor = vec4(color, 1.0);
    //FragColor = texture(material.diffuseTexture, TexCoords);
    // FragColor = vec4(texture(material.normalTexture, TexCoords).rgb, 1.0);
    // FragColor = vec4(texture(material.specularTexture, TexCoords).rgb, 1.0);

}
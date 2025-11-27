#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

uniform mat4 uLightProj;
uniform mat4 uLightView;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

void main()
{
    FragPos = vec3(uModel * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    
    FragPosLightSpace = uLightProj * uLightView * vec4(FragPos, 1.0);

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCameraPosition;
uniform vec3 uLightColor;
uniform vec3 uLightPosition;

uniform sampler2D uShadowMap;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 lightDirection)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(uShadowMap, projCoords.xy).r;   
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(Normal, lightDirection)), 0.003);  
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;  

    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

vec4 directionalLight() {
    vec3 ambientLight = 0.5f * uLightColor;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(uLightPosition);
    float diff = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = diff * uLightColor;

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(uCameraPosition - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
    float specular = specAmount * specularLight;

    float shadow = ShadowCalculation(FragPosLightSpace, lightDirection);
    vec3 color = uColor * (ambientLight + (1.0 - shadow) * (diffuse + specular));
    return vec4(color, 1.0);
}

void main()
{
    //vec4 color = vec4(1.0, 0.5, 0.5, 1.0);
    //FragColor = color;
    FragColor = directionalLight();
}
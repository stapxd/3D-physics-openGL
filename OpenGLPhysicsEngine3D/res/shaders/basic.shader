#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(uModel * vec4(aPos, 1.0));

    Normal = mat3(transpose(inverse(uModel))) * aNormal;

    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uCameraPosition;

in vec3 FragPos;
in vec3 Normal;

vec4 directionalLight() {
    vec3 LightColor = vec3(1.0, 1.0, 1.0);

    float ambientLight = 0.5f;

    vec3 normal = normalize(Normal);
    vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float specularLight = 0.5f;
    vec3 viewDirection = normalize(uCameraPosition - FragPos);
    vec3 reflectionDirection = reflect(-lightDirection, normal);
    float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 32);
    float specular = specAmount * specularLight;

    vec3 color = uColor * (diffuse + ambientLight) + LightColor * specular;
    return vec4(color, 1.0);
}

void main()
{
    //vec4 color = vec4(1.0, 0.5, 0.5, 1.0);
    //FragColor = color;
    FragColor = directionalLight();
}
#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;

out vec3 color;

void main()
{
    color = aColor;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

out vec4 FragColor;

in vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}
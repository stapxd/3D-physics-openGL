#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;

uniform mat4 uLightProj;
uniform mat4 uLightView;
uniform mat4 uModel;

void main()
{
    gl_Position = uLightProj * uLightView * uModel * vec4(aPos, 1.0);
}

#shader fragment
#version 460 core

void main()
{

}
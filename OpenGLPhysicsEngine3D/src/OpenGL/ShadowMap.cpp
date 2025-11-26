#include "ShadowMap.h"
#include <iostream>

#include <GL/glew.h>

#include "Application/Renderer.h"

ShadowMap::ShadowMap()
{
    GLCall(glGenFramebuffers(1, &m_RendererID));
    Initialize();
}

ShadowMap::~ShadowMap()
{
    GLCall(glDeleteFramebuffers(1, &m_RendererID));
}

void ShadowMap::Bind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID));
}

void ShadowMap::UnBind() const
{
    GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void ShadowMap::Initialize()
{
    GLCall(glGenTextures(1, &m_DepthMap));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthMap));

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        m_ShadowWidth, m_ShadowHeight,
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    Bind();

    GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMap, 0));
    GLCall(glDrawBuffer(GL_NONE));
    GLCall(glReadBuffer(GL_NONE));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "[OpenGL Error] (ShadowMap FBO ERROR)\n";

    UnBind();
}

void ShadowMap::BindDepthMap()
{
	GLCall(glBindTexture(GL_TEXTURE_2D, m_DepthMap));
}
#include "VertexBuffer.h"
#include <GL/glew.h>
#include "Application/Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int type)
{
	m_Type = type;

	GLCall(glGenBuffers(1, &m_RendererID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, m_Type));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::ChangeData(const void* data, unsigned int size)
{
	if (m_Type != GL_DYNAMIC_DRAW) return;

	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void VertexBuffer::UnBind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

#include "VertexArray.h"
#include <GL/glew.h>
#include "Application/Renderer.h"
#include <iostream>
VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	GLCall(glBindVertexArray(m_RendererID));
	m_LastAttributeIndex = 0;
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::UnBind() const
{
	GLCall(glBindVertexArray(0));
}

void VertexArray::AddBufferLayout(const VertexBuffer& buffer, const VertexLayout& layout)
{
	Bind();
	buffer.Bind();

	unsigned int offset = 0;
	const auto elements = layout.GetElements();

	for (size_t i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		GLCall(glEnableVertexAttribArray(m_LastAttributeIndex));
		GLCall(glVertexAttribPointer(m_LastAttributeIndex, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.count * VertexLayoutElement::GetSizeOfType(element.type);

		m_LastAttributeIndex++;
	}
}

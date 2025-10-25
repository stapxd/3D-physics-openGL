#pragma once
#include "VertexBuffer.h"
#include "VertexLayout.h"

class VertexArray
{
private:
	unsigned int m_RendererID;
	unsigned int m_LastAttributeIndex;
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void UnBind() const;

	void AddBufferLayout(const VertexBuffer& buffer, const VertexLayout& layout);
};


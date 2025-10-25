#pragma once
#include <GL/glew.h>
#include <vector>
#include "Application/Renderer.h"

struct VertexLayoutElement {
	unsigned int  count;
	unsigned int  type;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type)
		{
			case GL_FLOAT:			 return 4;
			case GL_UNSIGNED_INT:	 return 4;
			case GL_UNSIGNED_BYTE:	 return 1;
		}
		ASSERT(false);
		return 0;
	}

	VertexLayoutElement(unsigned int c, unsigned int t, unsigned char n) 
		: count(c), type(t), normalized(n)
	{}
};

class VertexLayout
{
private:
	unsigned int m_Stride = 0;
	std::vector<VertexLayoutElement> m_Elements;

public:
	template<typename T>
	void Push(unsigned int count) { static_assert(false); }

	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({ count, GL_FLOAT, GL_FALSE });
		m_Stride += count * sizeof(float);
	}

	template<>
	void Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
		m_Stride += count * sizeof(unsigned int);
	}

	inline const std::vector<VertexLayoutElement>& GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};


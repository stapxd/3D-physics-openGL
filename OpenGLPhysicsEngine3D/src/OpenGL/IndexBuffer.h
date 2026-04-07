#pragma once
class IndexBuffer // Index Buffer
{
private:
	unsigned int m_RendererID;
public:
	IndexBuffer(const void* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;
};


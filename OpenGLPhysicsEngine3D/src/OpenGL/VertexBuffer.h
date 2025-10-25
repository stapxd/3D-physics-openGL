#pragma once
class VertexBuffer
{
private:
	unsigned int m_RendererID;
	unsigned int m_Type;
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int type);
	~VertexBuffer();

	void ChangeData(const void* data, unsigned int size);

	void Bind() const;
	void UnBind() const;
};


#pragma once
class FrameBuffer
{
protected:
	unsigned int m_RendererID;

public:
	FrameBuffer();
	~FrameBuffer();

	void Bind() const;
	void UnBind() const;

protected:
	virtual void Initialize();
};


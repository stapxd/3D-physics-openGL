#pragma once

#include "FrameBuffer.h"

class ShadowMap
{
protected:
	unsigned int m_RendererID;

public:
	ShadowMap();
	~ShadowMap();

	void Bind() const;
	void UnBind() const;

	void BindDepthMap();

	unsigned int GetShadowWidth() const { return m_ShadowWidth; }
	unsigned int GetShadowHeight() const { return m_ShadowHeight; }
private:
	const unsigned int m_ShadowWidth = 4096, m_ShadowHeight = 4096;
	unsigned int m_DepthMap;

protected:
	virtual void Initialize();
};


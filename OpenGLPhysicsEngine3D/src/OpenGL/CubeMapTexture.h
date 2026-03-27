#pragma once

#include <vector>
#include <string>

class CubeMapTexture
{
public:
	CubeMapTexture(const std::vector<std::string>& files);
	~CubeMapTexture();

	void Bind(int slot = 0) const;
	void UnBind() const;

private:
	unsigned int m_RendererID;
};


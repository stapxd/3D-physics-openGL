#pragma once

#include <filesystem>
#include <iostream>
#include <vector>

#include "OpenGL/CubeMapTexture.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/IndexBuffer.h"

namespace fs = std::filesystem;

class SkyBox
{
public:
	SkyBox();
	SkyBox(const std::string& directory);

	void Init(const std::string& directory);

	void Draw();

private:
	bool isImage(const fs::path& path);

private:
	std::unique_ptr<CubeMapTexture> m_CubeMapTexture;

	std::vector<unsigned int> m_Indices;

	std::unique_ptr<VertexArray> m_VAO;
	std::unique_ptr<VertexBuffer> m_VBO;
	std::unique_ptr<IndexBuffer> m_IBO;
};


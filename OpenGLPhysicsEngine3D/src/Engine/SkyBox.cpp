#include "SkyBox.h"

#include <map>

SkyBox::SkyBox(const std::string& directory)
{
	Init(directory);
}

SkyBox::SkyBox() = default;

void SkyBox::Init(const std::string& directory)
{
	std::map<std::string, std::string> fileMap;
	for (const auto& entry : fs::directory_iterator(directory)) {
		if (entry.is_regular_file() && isImage(entry.path())) {
			std::string stem = entry.path().stem().string();
			std::transform(stem.begin(), stem.end(), stem.begin(), ::tolower);
			fileMap[stem] = entry.path().string();
		}
	}

	std::vector<std::string> faces = {
		fileMap["right"],
		fileMap["left"],
		fileMap["top"],
		fileMap["bottom"],
		fileMap["front"],
		fileMap["back"]
	};

	m_CubeMapTexture = std::make_unique<CubeMapTexture>(faces);

	std::vector<float> vertices = {
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
	};

	m_Indices = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		0, 4, 7,  7, 3, 0,
		1, 5, 6,  6, 2, 1,
		3, 2, 6,  6, 7, 3,
		0, 1, 5,  5, 4, 0,
	};

	m_VAO = std::make_unique<VertexArray>();

	m_VBO = std::make_unique<VertexBuffer>(vertices.data(), sizeof(float) * vertices.size(), GL_STATIC_DRAW);

	m_IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());

	VertexLayout layout;
	layout.Push<float>(3);

	m_VAO->AddBufferLayout(*m_VBO, layout);

	m_VAO->UnBind();
	m_VBO->UnBind();
	m_IBO->UnBind();
}

void SkyBox::Render()
{
	GLCall(glDepthFunc(GL_LEQUAL));

	m_VAO->Bind();
	m_CubeMapTexture->Bind();
	GLCall(glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, NULL));
	m_CubeMapTexture->UnBind();
	m_VAO->UnBind();

	GLCall(glDepthFunc(GL_LESS));
}

bool SkyBox::isImage(const fs::path& path)
{
	std::string ext = path.extension().string();

	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	return ext == ".jpg" || ext == ".jpeg" ||
		ext == ".png" || ext == ".bmp" ||
		ext == ".tga";
}

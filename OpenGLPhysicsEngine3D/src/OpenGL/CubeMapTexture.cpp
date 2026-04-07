#include "CubeMapTexture.h"

#include <stb/stb_image.h>
#include "Application/Core.h"

CubeMapTexture::CubeMapTexture(const std::vector<std::string>& faces)
{
	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));

	int width, height, BPP;

	if (faces.size() == 6) {
		for (size_t i = 0; i < faces.size(); i++) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &BPP, 4);
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
			stbi_image_free(data);
		}

		GLCall(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
	}
	else {
		unsigned int color = 0x3e3d7dff;

		for (size_t i = 0; i < 6; i++) {
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color));
		}
	}

	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

CubeMapTexture::~CubeMapTexture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void CubeMapTexture::Bind(int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID));
}

void CubeMapTexture::UnBind() const
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

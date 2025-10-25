#pragma once

#include <iostream>
#include <unordered_map>

#include "glm/glm.hpp"

struct ShaderProgramSourse {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_Filepath;
	unsigned int m_RendererID;
	mutable std::unordered_map<std::string, int> m_UniformLocationCash;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void UnBind() const;

	void SetUniform3f(const std::string& name, float v0, float v1, float v2) const;
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) const;
	void SetUniformBool(const std::string& name, bool value) const;
	void SetUniform1i(const std::string& name, int value) const;
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix) const;
private:
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	ShaderProgramSourse ParseShader();
	int GetUniformLocation(const std::string& name) const;
};


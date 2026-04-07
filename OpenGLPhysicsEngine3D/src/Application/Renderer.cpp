#include "Renderer.h"
#include <iostream>

// Renderer

void Renderer::BeginScene(const Camera& camera, const Shader& shader)
{
	shader.Bind();
	shader.SetUniformMat4f("uProj", camera.GetProjection());
	shader.SetUniformMat4f("uView", camera.GetView());
}

void Renderer::BeginSceneLight(const Camera& camera, const Shader& shader, const LightData& lightData)
{
	shader.Bind();
	shader.SetUniformMat4f("uProj", camera.GetProjection());
	shader.SetUniformMat4f("uView", camera.GetView());
		  
	shader.SetUniformMat4f("uLightProj", lightData.lightProjection);
	shader.SetUniformMat4f("uLightView", lightData.lightView);
		  
	shader.SetUniform3f("uLightColor", lightData.lightColor.r, lightData.lightColor.g, lightData.lightColor.b);
	shader.SetUniform3f("uLightPosition", lightData.lightPosition.x, lightData.lightPosition.y, lightData.lightPosition.z);
}

void Renderer::SetShadowLigntUniforms(const ShadowMap& shadowMap, const Camera& camera, const Shader& shader)
{
	glActiveTexture(GL_TEXTURE0);
	shadowMap.BindDepthMap();
	shader.SetUniform1i("uShadowMap", 0);

	glm::vec3 camPos = camera.GetPosition();
	shader.SetUniform3f("uCameraPosition", camPos.x, camPos.y, camPos.z);
}

void Renderer::DrawMesh(const Shader& shader, Mesh& mesh)
{
	mesh.Draw(shader);
}

void Renderer::DrawMesh(const Shader& shader, Mesh& mesh, glm::vec3 color)
{
	shader.SetUniform3f("uColor", color.r, color.g, color.b);

	mesh.Draw(shader);
}

void Renderer::EndScene(const Shader& shader)
{
	shader.UnBind();
}

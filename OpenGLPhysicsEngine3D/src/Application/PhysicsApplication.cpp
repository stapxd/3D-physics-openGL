#include "PhysicsApplication.h"
#include <iostream>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Enumerators/EntityTypes.h"

void PhysicsApplication::Start()
{
	// Variables initialization
	m_Camera = std::make_unique<Camera>(m_Width, m_Height);
	
	m_Manager.AddEntity(EntityTypes::StaticCube, { false });
	m_Manager.AddEntity(EntityTypes::Cube, { true });
	/*m_Cubes.reserve(2);
	m_Cubes.push_back(std::make_unique<Cube>());
	m_Cubes.push_back(std::make_unique<Cube>());
	m_Cubes[1]->Translate(glm::vec3(0.0f, 0.0f, -10.0f));*/

	m_Axes = std::make_unique<Axes>();
	m_Axes->Scale(glm::vec3(100, 100, 100));

	m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");
	m_AxisShader = std::make_unique<Shader>("res/shaders/axis.shader");

	m_Shader->UnBind();
	m_AxisShader->UnBind();
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.102f, 0.204f, 0.349f, 1.0f);
}

void PhysicsApplication::Update(float deltaTime)
{
    // Inputs
	m_Camera->Inputs(m_Window, deltaTime);
	
	//m_PhysicsWorld.Update(m_Cubes);

	//
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("uProj", m_Camera->GetProjection());
	m_Shader->SetUniformMat4f("uView", m_Camera->GetView());
	
	glm::vec3 camPos = m_Camera->GetPosition();
	m_Shader->SetUniform3f("uCameraPosition", camPos.x, camPos.y, camPos.z);

	for (auto& entity : m_Manager.GetEntities()) {
		ObjectProperties properties = entity.second.GetProperties();

		m_Shader->SetUniform3f("uColor", properties.color.x, properties.color.y, properties.color.z);

		entity.second->Scale(properties.transform.scale);
		entity.second->Rotate(properties.transform.rotation);
		entity.second->Translate(properties.transform.translation);

		entity.second->Draw(*m_Shader);
	}

	m_Shader->UnBind();
	/*m_Cubes[0]->Scale(m_Scale);
	m_Cubes[0]->Rotate(m_Rotation);
	m_Cubes[0]->Translate(m_Translation);

	m_Cubes[1]->Scale(m_Scale1);
	m_Cubes[1]->Rotate(m_Rotation1);
	m_Cubes[1]->Translate(m_Translation1);*/
	
	//Render
	/*m_Cubes[0]->Draw(*m_Shader);
	m_Cubes[1]->Draw(*m_Shader);*/


	m_AxisShader->Bind();
	m_AxisShader->SetUniformMat4f("uProj", m_Camera->GetProjection());
	m_AxisShader->SetUniformMat4f("uView", m_Camera->GetView());

	m_Axes->Draw(*m_AxisShader);
}

void PhysicsApplication::Inputs(float deltaTime)
{
	// on_size() -> m_Camera->SetViewport(width, height);

#if false
	IEntity* entity = m_PhysicsWorld.GetEntity(0);

	float forceMagnitude = 5.0f;

	float dx = 0, dy = 0;

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
		dy = 1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
		dx = -1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
		dy = -1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
		dx = 1;
	}

	if (dx != 0 || dy != 0) {
		glm::vec2 forceDirection = glm::normalize(glm::vec2(dx, dy));
		glm::vec2 force = forceDirection * forceMagnitude;
		entity->ApplyForce(force);
	}

	if (glfwGetKey(m_Window, GLFW_KEY_R) == GLFW_PRESS) {
		entity->Rotate(3.1415f / 8 * deltaTime);
	}
#endif
}

void PhysicsApplication::HandleOnSize(int width, int height)
{
	//printf("w: %d \n h: %d\n", width, height);
	glViewport(0, 0, width, height);
	m_Camera->SetViewport(width, height);
}

void PhysicsApplication::HandleOnMouseMove(double xpos, double ypos)
{
	m_Camera->OnMouseMove(xpos, ypos);
}

void PhysicsApplication::ShowImGui()
{
	Transform& objectTransform = m_Manager.FindEntity(1).GetProperties().transform;

	ImGui::Begin("Cube1");

	ImGui::Text("Transform"); // Transform
	ImGui::InputFloat3("Traslation", (float*)&objectTransform.translation);
	ImGui::InputFloat3("Rotation", (float*)&objectTransform.rotation);
	ImGui::InputFloat3("Scale", (float*)&objectTransform.scale);

	ImGui::Separator();

	ImGui::Text("Material"); // Material
	ImGui::ColorEdit3("Color", &m_Manager.FindEntity(1).GetProperties().color[0]);

	ImGui::End();
}

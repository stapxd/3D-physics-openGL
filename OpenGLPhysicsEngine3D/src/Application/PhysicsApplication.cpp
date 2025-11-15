#include "PhysicsApplication.h"
#include <iostream>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

void PhysicsApplication::Start()
{
	// Variables initialization
	m_Camera = std::make_unique<Camera>(m_Width, m_Height);
	
	m_PhysicsWorld->AddEntity(EntityTypes::StaticCube, { false });
	m_PhysicsWorld->AddEntity(EntityTypes::Cube, { true });

	m_Axes = std::make_unique<Axes>();
	m_Axes->Scale(glm::vec3(100, 100, 100));

	m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");
	m_AxisShader = std::make_unique<Shader>("res/shaders/axis.shader");

	m_Shader->UnBind();
	m_AxisShader->UnBind();
	
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

	for (auto& entity : m_PhysicsWorld.GetEntities()) {
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
	if(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
		&& glfwGetKey(m_Window, GLFW_KEY_P) == GLFW_PRESS
		&& !m_LMButtonIsPressed) {
		double xPos, yPos;
		GetCursorPosition(&xPos, &yPos);
		//m_SelectedEntity = m_PhysicsWorld.SelectEntityWithScreenPosition(xPos, yPos, m_Width, m_Height, m_Camera.get());
		m_SelectedEntity = &m_PhysicsWorld->FindEntity(1);
		m_LMButtonIsPressed = true;
	}
	else if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		m_LMButtonIsPressed = false;
	}
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
	if (m_SelectedEntity) {
		Transform& objectTransform = m_SelectedEntity->GetProperties().transform;

		std::string name = "Entity #" + std::to_string(m_SelectedEntity->GetId());

		ImGui::Begin(name.c_str());

		ImGui::Text("Transform"); // Transform
		ImGui::InputFloat3("Traslation", (float*)&objectTransform.translation);
		ImGui::InputFloat3("Rotation", (float*)&objectTransform.rotation);
		ImGui::InputFloat3("Scale", (float*)&objectTransform.scale);

		ImGui::Separator();

		ImGui::Text("Material"); // Material
		ImGui::ColorEdit3("Color", &m_SelectedEntity->GetProperties().color[0]);

		ImGui::End();
	}
	else {
		ImGui::Begin("Menu");
		ImGui::InputFloat3("Spawn Point", (float*)&m_SpawnPoint[0]);
		if (ImGui::Button("Spawn"))
			std::cout << "Spawned";
		ImGui::End();
	}
}

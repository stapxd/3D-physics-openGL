#include "PhysicsApplication.h"
#include <iostream>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Structures/ObjectProperties.h"

void PhysicsApplication::Start()
{
	// Variables initialization
	m_Camera = std::make_unique<Camera>(m_Width, m_Height);
	
	/*m_PhysicsWorld->AddEntity(EntityTypes::Cube);
	m_PhysicsWorld->AddEntity(EntityTypes::Cube);*/

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
	
	// Update world
	m_PhysicsWorld.Update(deltaTime, 1);

	// Rendering
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("uProj", m_Camera->GetProjection());
	m_Shader->SetUniformMat4f("uView", m_Camera->GetView());
	
	glm::vec3 camPos = m_Camera->GetPosition();
	m_Shader->SetUniform3f("uCameraPosition", camPos.x, camPos.y, camPos.z);

	for (auto& entity : m_PhysicsWorld.GetEntities()) {
		ObjectProperties properties = entity.second.GetProperties();

		m_Shader->SetUniform3f("uColor", properties.color.x, properties.color.y, properties.color.z);

		entity.second->Draw(*m_Shader);
	}

	m_Shader->UnBind();

	if (m_ShowAxes) {
		m_AxisShader->Bind();
		m_AxisShader->SetUniformMat4f("uProj", m_Camera->GetProjection());
		m_AxisShader->SetUniformMat4f("uView", m_Camera->GetView());

		m_Axes->Draw(*m_AxisShader);
	}
}

void PhysicsApplication::Inputs(float deltaTime)
{
	if(glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS
		&& glfwGetKey(m_Window, GLFW_KEY_P) == GLFW_PRESS
		&& !m_LMButtonIsPressed) {
		double xPos, yPos;
		GetCursorPosition(&xPos, &yPos);
		m_SelectedEntity = m_PhysicsWorld.SelectEntityWithScreenPosition(xPos, yPos, m_Width, m_Height, m_Camera.get());
		//m_SelectedEntity = &m_PhysicsWorld->FindEntity(1);
		m_LMButtonIsPressed = true;
	}
	else if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		m_LMButtonIsPressed = false;
	}

	float fM = .1f;
	if (m_SelectedEntity && !m_SelectedEntity->GetProperties().rigidbody.isStatic) {
		if (glfwGetKey(m_Window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(1.0f, 0.0f, 0.0f) * fM;
		}
		if (glfwGetKey(m_Window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(-1.0f, 0.0f, 0.0f) * fM;
		}
		if (glfwGetKey(m_Window, GLFW_KEY_UP) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(0.0f, 0.0f, 1.0f) * fM;
		}
		if (glfwGetKey(m_Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(0.0f, 0.0f, -1.0f) * fM;
		}
	}
}

void PhysicsApplication::HandleOnSize(int width, int height)
{
	//printf("w: %d \n h: %d\n", width, height);
	m_Width = width;
	m_Height = height;

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

		if (ImGui::CollapsingHeader("Transform")) { // Transform
			ImGui::DragFloat3("Traslation", (float*)&objectTransform.translation, 0.025f);
			ImGui::DragFloat3("Rotation", (float*)&objectTransform.rotation, 0.1f);
			ImGui::DragFloat3("Scale", (float*)&objectTransform.scale, 0.025f);
		}

		ImGui::Separator();

		if(ImGui::CollapsingHeader("Material")) { // Material
			ImGui::ColorEdit3("Color", &m_SelectedEntity->GetProperties().color[0]);
		}

		ImGui::Separator();

		if(ImGui::CollapsingHeader("Rigidbody")) { // Rigidbody
			ImGui::Checkbox("Static", &m_SelectedEntity->GetProperties().rigidbody.isStatic);
			ImGui::DragFloat("Mass", &m_SelectedEntity->GetProperties().rigidbody.mass, 0.02f, 0.01f, 1000.0f);
			ImGui::DragFloat("Restitution", &m_SelectedEntity->GetProperties().rigidbody.restitution, 0.1f, 0.1f, 1000.0f);
		}

		ImGui::End();
	}
	else {
		ImGui::Begin("Menu");
		if (ImGui::CollapsingHeader("Scene")) {
			ImGui::Checkbox("Show Axes", &m_ShowAxes);
			//ImGui::DragFloat3("Lighting position", ...);
		}
		if (ImGui::CollapsingHeader("Spawning")) {
			SelectEntityType();

			ImGui::Separator();

			ImGui::Text("Entity Parameters");

			Transform& transform = m_SpawnManager.GetParams().transform;
			Rigidbody3D& rigidbody = m_SpawnManager.GetParams().rigidbody;
			// vector not initializing
			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Scale", &transform.scale[0], 0.025f);
			}

			if (ImGui::CollapsingHeader("Rigidbody")) {
				ImGui::Checkbox("Static", &rigidbody.isStatic);
				ImGui::DragFloat("Mass", &rigidbody.mass, 0.02f, 0.01f, 1000.0f);
				ImGui::DragFloat("Restitution", &rigidbody.restitution, 0.1f, 0.1f, 1000.0f);
			}

			ImGui::Separator();

			// TODO: doesn't spawn in this point. Check why???
			ImGui::DragFloat3("Spawn Point", &m_SpawnManager.GetSpawnPointChangeable()[0], 0.1f);
			if (ImGui::Button("Spawn", ImVec2(-1, 0)))
				m_SpawnManager.Spawn(m_PhysicsWorld);
		}
		ImGui::End();
	}
}

void PhysicsApplication::SelectEntityType()
{
	const char* items[] = { "Cube" };

	int selectedType = static_cast<int>(m_SpawnManager.GetSelectedEntityType());
	if (ImGui::Combo("Select type", &selectedType, items, IM_ARRAYSIZE(items))) {
		m_SpawnManager.SetSelectedEntityType(static_cast<EntityTypes>(selectedType));
	}
}

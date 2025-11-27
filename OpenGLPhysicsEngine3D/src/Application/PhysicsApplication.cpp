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
	
	m_ShadowMap = std::make_unique<ShadowMap>();

	//m_PhysicsWorld->AddEntity(EntityTypes::Cube, m_Params1);
	//m_PhysicsWorld->AddEntity(EntityTypes::Cube, m_Params2);

	m_LightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, m_NearPlane, m_FarPlane);

	m_Axes = std::make_unique<Axes>();
	m_Axes->Scale(glm::vec3(100, 100, 100));

	m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");
	m_AxisShader = std::make_unique<Shader>("res/shaders/axis.shader");
	m_ShadowShader = std::make_unique<Shader>("res/shaders/shadow.shader");

	m_Shader->UnBind();
	m_AxisShader->UnBind();
	m_ShadowShader->UnBind();
	
	glClearColor(0.102f, 0.204f, 0.349f, 1.0f);
}

void PhysicsApplication::Update(float deltaTime)
{
    // Inputs
	m_Camera->Inputs(m_Window, deltaTime);
	
	// Update world
	m_PhysicsWorld.Update(deltaTime, 16);

	// Rendering
	glViewport(0, 0, m_ShadowMap->GetShadowWidth(), m_ShadowMap->GetShadowHeight());

	m_ShadowMap->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	RenderSceneDepthMap();
	glCullFace(GL_BACK);
	m_ShadowMap->UnBind();

	glViewport(0, 0, m_Width, m_Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_ShadowMap->BindDepthMap();
	RenderScene();
}

void PhysicsApplication::RenderScene()
{
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("uProj", m_Camera->GetProjection());
	m_Shader->SetUniformMat4f("uView", m_Camera->GetView());

	m_Shader->SetUniformMat4f("uLightProj", m_LightProjection);
	m_Shader->SetUniformMat4f("uLightView", m_LightView);

	m_Shader->SetUniform3f("uLightColor", m_LightColor.x, m_LightColor.y, m_LightColor.z);
	m_Shader->SetUniform3f("uLightPosition", m_LightPosition.x, m_LightPosition.y, m_LightPosition.z);

	glActiveTexture(GL_TEXTURE0);
	m_ShadowMap->BindDepthMap();
	m_Shader->SetUniform1i("uShadowMap", 0);

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
		m_AxisShader->UnBind();
	}
}

void PhysicsApplication::RenderSceneDepthMap()
{
	m_LightView = glm::lookAt(m_LightPosition,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	m_ShadowShader->Bind();
	m_ShadowShader->SetUniformMat4f("uLightProj", m_LightProjection);
	m_ShadowShader->SetUniformMat4f("uLightView", m_LightView);

	for (auto& entity : m_PhysicsWorld.GetEntities()) {
		ObjectProperties properties = entity.second.GetProperties();
		entity.second->Draw(*m_ShadowShader);
	}

	m_ShadowShader->UnBind();
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

	float fM = 20.0f;
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
		if (glfwGetKey(m_Window, GLFW_KEY_KP_0) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(0.0f, -1.0f, 0.0f) * fM;
		}
		if (glfwGetKey(m_Window, GLFW_KEY_KP_1) == GLFW_PRESS) {
			m_SelectedEntity->GetProperties().rigidbody.force = glm::vec3(0.0f, 1.0f, 0.0f) * fM;
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
			ImGui::Checkbox("Use Gravity", &m_SelectedEntity->GetProperties().rigidbody.useGravity);
			ImGui::DragFloat("Mass", &m_SelectedEntity->GetProperties().rigidbody.mass, 0.02f, 0.01f, 1000.0f);
			ImGui::DragFloat("Restitution", &m_SelectedEntity->GetProperties().rigidbody.restitution, 0.1f, 0.1f, 1000.0f);
		}

		ImGui::End();
	}
	else {
		ImGui::Begin("Menu");
		if (ImGui::CollapsingHeader("Scene")) {
			ImGui::Checkbox("Show Axes", &m_ShowAxes);
			if (ImGui::Button("Clear all entities"))
				m_PhysicsWorld->ClearAll();
			ImGui::DragFloat3("Lighting position", &m_LightPosition[0], 0.025f);
		}
		if (ImGui::CollapsingHeader("Spawning")) {
			SelectEntityType();

			ImGui::Separator();

			ImGui::Text("Entity Parameters");

			Transform& transform = m_SpawnManager.GetParams().transform;
			Rigidbody3D& rigidbody = m_SpawnManager.GetParams().rigidbody;

			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::DragFloat3("Scale", &transform.scale[0], 0.025f);
			}

			if (ImGui::CollapsingHeader("Rigidbody")) {
				ImGui::Checkbox("Static", &rigidbody.isStatic);
				ImGui::Checkbox("Use Gravity", &rigidbody.useGravity);
				ImGui::DragFloat("Mass", &rigidbody.mass, 0.02f, 0.01f, 1000.0f);
				ImGui::DragFloat("Restitution", &rigidbody.restitution, 0.1f, 0.1f, 1000.0f);
			}

			ImGui::Separator();

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

#include "PhysicsApplication.h"
#include <iostream>
#include <helpers.h>
#include <thread>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>
#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Structures/ObjectProperties.h"

#include "Objects/Sphere.h"


// TEMP ---------------------
#include "Physics/GJK.h"


PhysicsApplication::PhysicsApplication()
	: m_SaveManager(m_PhysicsWorld)
{
}

PhysicsApplication::~PhysicsApplication()
{
}

void PhysicsApplication::Start()
{
	// Variables initialization
	m_Camera = std::make_unique<Camera>(m_Width, m_Height);
	
	m_ShadowMap = std::make_unique<ShadowMap>();

	m_SkyBox.Init("res/skyboxes/field");

	//m_PhysicsWorld->AddEntity(EntityTypes::Cube, m_Params1);
	//m_PhysicsWorld->AddEntity(EntityTypes::Cube, m_Params2);

	m_LightData.lightProjection = glm::ortho(-m_LightData.orthoSize, m_LightData.orthoSize, -m_LightData.orthoSize, m_LightData.orthoSize, m_LightData.nearPlane, m_LightData.farPlane);

	m_Axes = std::make_unique<Axes>();
	m_Axes->Scale(glm::vec3(100, 100, 100));

	// Shaders
	m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");
	m_AxisShader = std::make_unique<Shader>("res/shaders/axis.shader");
	m_ShadowShader = std::make_unique<Shader>("res/shaders/shadow.shader");
	m_SkyBoxShader = std::make_unique<Shader>("res/shaders/skybox.shader");

	m_SolidColorShader = std::make_unique<Shader>("res/shaders/solidColor.shader");

	m_Shader->UnBind();
	m_AxisShader->UnBind();
	m_ShadowShader->UnBind();
	m_SkyBoxShader->UnBind();

	m_SolidColorShader->UnBind();
	
	m_PauseManager.Attach(&m_PhysicsWorld);





	m_OBBPointsVAO = std::make_unique<VertexArray>();
	m_OBBPointsVBO = std::make_unique<VertexBuffer>(nullptr, 64*3*sizeof(float), GL_DYNAMIC_DRAW);

	m_OBBLayout.Push<float>(3);
	m_OBBPointsVAO->AddBufferLayout(*m_OBBPointsVBO, m_OBBLayout);

	unsigned int incidesTriangles[12] = { 0,1,2, 0,2,3, 0,3,1, 1,2,3 };
	m_OBBPointsIBOTriangles = std::make_unique<IndexBuffer>((const void*)incidesTriangles, 12);


	m_OBBPointsVAO->UnBind();
	m_OBBPointsVBO->UnBind();
	m_OBBPointsIBOTriangles->UnBind();

	glClearColor(0.102f, 0.204f, 0.349f, 1.0f);
}

void PhysicsApplication::FixedUpdate(float fixedDeltaTime) {
	// Update world
	m_PhysicsWorld.Update(fixedDeltaTime, 10);
}

void PhysicsApplication::Update(float deltaTime)
{
	// Clearing
	m_PhysicsWorld->Flush();

    // Inputs
	m_Camera->Inputs(m_Window, deltaTime);
	
	// Update world
	//m_PhysicsWorld.Update(deltaTime, 1);

	// Rendering
	glViewport(0, 0, m_ShadowMap->GetShadowWidth(), m_ShadowMap->GetShadowHeight());

	m_ShadowMap->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderSceneDepthMap();
	m_ShadowMap->UnBind();

	glViewport(0, 0, m_Width, m_Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_ShadowMap->BindDepthMap();
	RenderScene();
}

void PhysicsApplication::RenderScene()
{
	Renderer::BeginSceneLight(*m_Camera, *m_Shader, m_LightData);
	Renderer::SetShadowLigntUniforms(*m_ShadowMap, *m_Camera, *m_Shader);

	for (auto& entity : m_PhysicsWorld.GetEntities()) {
		ObjectProperties properties = entity.second.GetProperties();
		Renderer::DrawMesh(*m_Shader, *(entity.second->GetMesh()), properties.color);
	}
	Renderer::EndScene(*m_Shader);

#ifdef COLLISION_POINTS_DEBUG
	Renderer::BeginScene(*m_Camera, *m_SolidColorShader);

	m_CollisionManifold = m_PhysicsWorld.GetCollisionManifold();
	size_t count = m_CollisionManifold.contactPoints.size();

	if (m_CollisionManifold.reference) {
		glDisable(GL_DEPTH_TEST);/*
		glm::vec3 v0 = m_CollisionManifold.reference->vertices[0];
		std::cout << v0.x << " " << v0.y << " " << v0.z << "\n";*/

		m_OBBPointsVBO->Bind();
		m_OBBPointsVBO->ChangeData((const void*)m_CollisionManifold.reference->vertices, 4 * sizeof(glm::vec3));
		m_OBBPointsVAO->Bind();

		m_SolidColorShader->SetUniform4f("uColor", 0.0f, 1.0f, 1.0f, 0.6f);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 4);
		glPointSize(1.0f);

		m_SolidColorShader->SetUniform4f("uColor", 0.0f, 0.1f, 0.8f, 0.6f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		m_OBBPointsVAO->UnBind();
		glEnable(GL_DEPTH_TEST);
	}

	if (m_CollisionManifold.incident) {
		glDisable(GL_DEPTH_TEST);/*
		glm::vec3 v0 = m_CollisionManifold.incident->vertices[0];
		std::cout << v0.x << " " << v0.y << " " << v0.z << "\n";*/

		m_OBBPointsVBO->Bind();
		m_OBBPointsVBO->ChangeData((const void*)m_CollisionManifold.incident->vertices, 4 * sizeof(glm::vec3));
		m_OBBPointsVAO->Bind();

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 0.0f, 1.0f, 0.6f);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, 4);
		glPointSize(1.0f);

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 0.8f, 0.1f, 0.6f);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		m_OBBPointsVAO->UnBind();
		glEnable(GL_DEPTH_TEST);
	}

	if (glm::length(m_CollisionManifold.normal) != 0) {
		glDisable(GL_DEPTH_TEST);/*
		glm::vec3 v0 = m_CollisionManifold.incident->vertices[0];
		std::cout << v0.x << " " << v0.y << " " << v0.z << "\n";*/
		std::vector<glm::vec3> normalToDraw = {
			{0.0f, 0.0f, 0.0f},
			m_CollisionManifold.normal
		};
		m_OBBPointsVBO->Bind();
		m_OBBPointsVBO->ChangeData((const void*)normalToDraw.data(), 2 * sizeof(glm::vec3));
		m_OBBPointsVAO->Bind();

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 0.8f, 0.1f, 0.6f);
		glDrawArrays(GL_LINES, 0, 2);
		m_OBBPointsVAO->UnBind();
		glEnable(GL_DEPTH_TEST);
	}
	if (count > 0) {

		std::cout << "Contact point count: " << count << "\n";
		
		/*glm::vec3 cp0 = m_CollisionManifold.contactPoints[0];
		std::cout << cp0.x << " " << cp0.y << " " << cp0.z << "\n";*/

		m_OBBPointsVBO->Bind();
		m_OBBPointsVBO->ChangeData(m_CollisionManifold.contactPoints.data(), count * sizeof(glm::vec3));
		m_OBBPointsVAO->Bind();

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 0.1f, 0.1f, 1.0f);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, count);

		m_OBBPointsVAO->UnBind();

		glPointSize(1.0f);
		glLineWidth(1.0f);
	}

	Renderer::EndScene(*m_SolidColorShader);
#endif

#ifdef GJK_DEBUG
	Renderer::BeginScene(*m_Camera, *m_SolidColorShader);
	for (auto& entity : m_PhysicsWorld.GetEntities()) {
		auto obb = entity.second->GetOBB();
		m_SolidColorShader->SetUniform4f("uColor", 0.9f, 0.9f, 0.9f, 1.0f);
		std::vector<glm::vec3> minkowskiPoints = m_PhysicsWorld.GetMinkowsiDiff();
		if (!minkowskiPoints.empty()) {
			m_OBBPointsVBO->Bind();
			m_OBBPointsVBO->ChangeData(minkowskiPoints.data(), minkowskiPoints.size() * sizeof(glm::vec3));

			m_OBBPointsVAO->Bind();
			glPointSize(5.0f);
			glDrawArrays(GL_POINTS, 0, (GLsizei)minkowskiPoints.size());
			glPointSize(1.0f);
		}

	}
	Renderer::EndScene(*m_SolidColorShader);


	Renderer::BeginScene(*m_Camera, *m_SolidColorShader);

	m_SimplexPoints = m_PhysicsWorld.GetSimplexPoints();
	unsigned int count = (unsigned int)m_SimplexPoints.size();

	if (count > 0) {

		m_OBBPointsVBO->Bind();
		m_OBBPointsVBO->ChangeData(m_SimplexPoints.data(), count * sizeof(glm::vec3));
		m_OBBPointsVAO->Bind();

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 0.1f, 0.1f, 1.0f);
		glPointSize(10.0f);
		glDrawArrays(GL_POINTS, 0, count);

		m_SolidColorShader->SetUniform4f("uColor", 1.0f, 1.0f, 0.0f, 0.5f);

		if (count == 2) {
			glLineWidth(2.0f);
			glDrawArrays(GL_LINES, 0, 2);
		}
		else if (count == 3) {
			m_SolidColorShader->SetUniform4f("uColor", 0.0f, 0.4f, 0.8f, 0.4f);
			glDrawArrays(GL_TRIANGLES, 0, 3);

			m_SolidColorShader->SetUniform4f("uColor", 1.0f, 1.0f, 0.0f, 1.0f);
			glDrawArrays(GL_LINE_LOOP, 0, 3);
		}
		else if (count == 4) {
			m_OBBPointsIBOTriangles->Bind();

			glDisable(GL_CULL_FACE);

			m_SolidColorShader->SetUniform4f("uColor", 1.0f, 1.0f, 0.0f, 1.0f);
			glLineWidth(2.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			m_SolidColorShader->SetUniform4f("uColor", 0.0f, 1.0f, 0.5f, 0.2f);
			glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);

			glEnable(GL_CULL_FACE);
			m_OBBPointsIBOTriangles->UnBind();
		}

		m_OBBPointsVAO->UnBind();

		glPointSize(1.0f);
		glLineWidth(1.0f);
	}

	Renderer::EndScene(*m_SolidColorShader);
#endif

	if (m_ShowAxes) {
		Renderer::BeginScene(*m_Camera, *m_AxisShader);
		m_Axes->Draw(*m_AxisShader);
		Renderer::EndScene(*m_AxisShader);
	}

	if (m_ShowSkyBox) {
		Renderer::BeginScene(*m_Camera, *m_SkyBoxShader);
		m_SkyBoxShader->SetUniform1i("uSkyBox", 0);
		m_SkyBox.Draw();
		Renderer::EndScene(*m_SkyBoxShader);
	}
}

void PhysicsApplication::RenderSceneDepthMap()
{
	m_LightData.lightView = glm::lookAt(m_LightData.lightPosition,
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

	m_ShadowShader->Bind();
	m_ShadowShader->SetUniformMat4f("uLightProj", m_LightData.lightProjection);
	m_ShadowShader->SetUniformMat4f("uLightView", m_LightData.lightView);

	for (auto& entity : m_PhysicsWorld.GetEntities()) {
		ObjectProperties properties = entity.second.GetProperties();
		entity.second->Draw(*m_ShadowShader);
	}

	m_ShadowShader->UnBind();
}

void PhysicsApplication::Inputs(float deltaTime)
{
	// Saving 
	if (glfwGetKey(m_Window, GLFW_KEY_F5) == GLFW_PRESS && !m_F5Pressed) {
		if (m_PauseManager.GetCurrentState() != ApplicationStates::Play) {
			try {
				m_SaveManager.Save(GetWindow());
			}
			catch (const std::string& e) {
				std::cout << "Saving error (" << e << ")\n";
			}
		}
		m_F5Pressed = true;
	}
	else if (glfwGetKey(m_Window, GLFW_KEY_F5) == GLFW_RELEASE) {
		m_F5Pressed = false;
	}

	if (glfwGetKey(m_Window, GLFW_KEY_F6) == GLFW_PRESS && !m_F6Pressed) {
		try {
			m_SaveManager.Load(GetWindow());
		}
		catch (const std::runtime_error e) {
			std::cout << "Loading error (" << e.what() << ")\n";
		}
		m_PauseManager.ChangeState(ApplicationStates::Stop);
		m_F6Pressed = true;
	}
	else if (glfwGetKey(m_Window, GLFW_KEY_F5) == GLFW_RELEASE) {
		m_F6Pressed = false;
	}

	// Pause handling
	if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS && !m_KeyPressed) {
		if (m_PauseManager.GetCurrentState() == ApplicationStates::Paused)
			m_PauseManager.ChangeState(ApplicationStates::Play);
		else if (m_PauseManager.GetCurrentState() == ApplicationStates::Play)
			m_PauseManager.ChangeState(ApplicationStates::Paused);

		m_KeyPressed = true;
	}
	else if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		m_KeyPressed = false;
	}

	// Mouse click
	if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS  // selection
		&& glfwGetKey(m_Window, GLFW_KEY_P) == GLFW_PRESS
		&& !m_LMButtonIsPressed) {
		double xPos, yPos;
		GetCursorPosition(&xPos, &yPos);
		glm::vec3 rayDir;
		m_SelectedEntity = m_PhysicsWorld.SelectEntityWithScreenPosition(xPos, yPos, m_Width, m_Height, m_Camera.get(), rayDir, 0);
		m_LMButtonIsPressed = true;
	}
	else if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		m_LMButtonIsPressed = false;
	}

	if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS  // hitting object at point
		&& glfwGetKey(m_Window, GLFW_KEY_H) == GLFW_PRESS
		&& !m_LMButtonIsPressed) {
		double xPos, yPos;
		GetCursorPosition(&xPos, &yPos);
		glm::vec3 hitPoint;
		glm::vec3 rayDir;
		Entity* entity = m_PhysicsWorld.SelectEntityWithScreenPosition(xPos, yPos, m_Width, m_Height, m_Camera.get(), rayDir, &hitPoint);
		if (entity != nullptr) {
			m_PhysicsWorld.AddForceAtPoint(*entity, hitPoint, rayDir, 0.6f);
			m_LMButtonIsPressed = true;
		}
	}
	else if (glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		m_LMButtonIsPressed = false;
	}

	// Adding force to selected entity
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
	if (m_PauseManager.GetCurrentState() == ApplicationStates::Paused) {
		ImGui::Begin("Pause");
		ImGui::Text("Engine is Paused!");
		ImGui::End();
	}

	ShowMainMenu();

	if (m_ShowSpawningMenu) {
		ShowSpawningMenu();
	}

	if (m_SelectedEntity) {
		ShowEntityMenu();
	}
	
}

void PhysicsApplication::ShowMainMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("Save", "F5")) {
				if (m_PauseManager.GetCurrentState() != ApplicationStates::Play) {
					try {
						m_SaveManager.Save(GetWindow());
					}
					catch (const std::runtime_error& e) {
						helpers::logError("SaveManager::Load", e.what());
					}
				}
			}
			if (ImGui::MenuItem("Open...", "F6")) {
				try {
					m_SaveManager.Load(GetWindow());
					m_SelectedEntity = nullptr;
				}
				catch (const std::runtime_error& e) {
					helpers::logError("SaveManager::Load", e.what());
				}
				m_PauseManager.ChangeState(ApplicationStates::Stop);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("General"))
		{

			ImGui::Checkbox("Show SkyBox", &m_ShowSkyBox);
			ImGui::Checkbox("Show Axes", &m_ShowAxes);

			bool isVaccum = m_PhysicsWorld.GetIsVaccum();
			if (ImGui::Checkbox("Vacuum Mode", &isVaccum)) {
				m_PhysicsWorld.SetIsVaccum(isVaccum);
			}

			ImGui::DragFloat3("Lighting position", &m_LightData.lightPosition[0], 0.025f);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Objects"))
		{
			if (ImGui::Button("Clear all entities", ImVec2(-1, 0))) {
				m_PhysicsWorld->ClearAll();
				m_SelectedEntity = nullptr;
			}

			ImGui::Checkbox("Toggle Spawning Menu", &m_ShowSpawningMenu);

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Simulation")) {
			if (ImGui::MenuItem("Play")) {
				if (m_PauseManager.GetCurrentState() != ApplicationStates::Play) {
					m_PauseManager.ChangeState(ApplicationStates::Play);
					m_PhysicsWorld->SetSnapshots();
				}
			}
			if (ImGui::MenuItem("Stop")) {
				if(m_PauseManager.GetCurrentState() != ApplicationStates::Stop)
					m_PhysicsWorld->ReturnToSnapshot();

				m_PauseManager.ChangeState(ApplicationStates::Stop);
			}
			if (ImGui::MenuItem("Pause")) {
				if (m_PauseManager.GetCurrentState() == ApplicationStates::Paused)
					m_PauseManager.ChangeState(ApplicationStates::Play);
				else if (m_PauseManager.GetCurrentState() == ApplicationStates::Play)
					m_PauseManager.ChangeState(ApplicationStates::Paused);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void PhysicsApplication::ShowEntityMenu()
{
	Transform& objectTransform = m_SelectedEntity->GetProperties().transform;

	std::string name = "Entity #" + std::to_string(m_SelectedEntity->GetId());

	ImGui::Begin(name.c_str());

	if (ImGui::CollapsingHeader("State")) {     // State
		if (ImGui::Button("Delete", ImVec2(-1, 0))) {
			m_PhysicsWorld->DeleteEntity(m_SelectedEntity->GetId());
			m_SelectedEntity = nullptr;
			ImGui::End();
			return;
		}

		ImGui::Checkbox("Is Sleeping", &m_SelectedEntity->GetIsSleeping());
		ImGui::DragFloat("Lin Threshold", &m_SelectedEntity->GetLinVThreshold(), 0.002f, 0.0f, 1.0f);
		ImGui::DragFloat("Ang Threshold", &m_SelectedEntity->GetAngVThreshold(), 0.002f, 0.0f, 1.0f);

		ImGui::Text("%.6f < %.6f", glm::dot(m_SelectedEntity->GetProperties().rigidbody.linearVelocity, m_SelectedEntity->GetProperties().rigidbody.linearVelocity), m_SelectedEntity->GetLinVThreshold() * m_SelectedEntity->GetLinVThreshold());
		ImGui::Text("%.6f < %.6f", glm::dot(m_SelectedEntity->GetProperties().rigidbody.angularVelocity, m_SelectedEntity->GetProperties().rigidbody.angularVelocity), m_SelectedEntity->GetAngVThreshold() * m_SelectedEntity->GetAngVThreshold());
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Transform")) { // Transform
		ImGui::DragFloat3("Traslation", (float*)&objectTransform.translation, 0.025f);

		//ImGui::DragFloat3("Rotation", (float*)&objectTransform.rotation, 0.1f); // depricated
		if (ImGui::DragFloat3("Rotation", (float*)&objectTransform.rotation, 0.1f)) {
			objectTransform.orientation = glm::quat(glm::radians(objectTransform.rotation));
		}
		if (!ImGui::IsItemActive()) {
			glm::vec3 currentEuler = glm::degrees(glm::eulerAngles(objectTransform.orientation));

			if (glm::distance(objectTransform.rotation, currentEuler) > 0.01f) {
				objectTransform.rotation = currentEuler;
			}
		}

		EntityTypes type = m_SelectedEntity->GetType();
		switch (type)
		{
		case EntityTypes::Cube:
			{
				if (ImGui::DragFloat3("Scale", &objectTransform.scale[0], 0.025f)) {
					m_SelectedEntity->GetEntity()->ApplyTransform(objectTransform);

					m_SelectedEntity->GetEntity()->EstimateInertiaTensor(m_SelectedEntity->GetProperties().rigidbody);
				}
				break;
			}
		case EntityTypes::Sphere:
			{
				Sphere* sphere = (Sphere*)m_SelectedEntity->GetEntity();

				float currentRadius = objectTransform.scale.x;

				if (ImGui::DragFloat("Radius", &currentRadius, 0.025f, 0.5f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {

					objectTransform.scale = glm::vec3(currentRadius);

					sphere->SetRadius(currentRadius);

					m_SelectedEntity->GetEntity()->ApplyTransform(objectTransform);

					m_SelectedEntity->GetEntity()->EstimateInertiaTensor(m_SelectedEntity->GetProperties().rigidbody);
				}
				break;
			}
		default:
			helpers::logError("PhysicsApplication::ShowSpawningMenu", "invalid entity type!");
		}
	}

	ImGui::Separator();

	if (ImGui::CollapsingHeader("Material")) { // Material
		ImGui::ColorEdit3("Color", &m_SelectedEntity->GetProperties().color[0]);
	}

	ImGui::Separator();

	Rigidbody3D& rb = m_SelectedEntity->GetProperties().rigidbody;

	if (ImGui::CollapsingHeader("Rigidbody")) { // Rigidbody
		ImGui::Checkbox("Static", &rb.isStatic);
		ImGui::Checkbox("Use Gravity", &rb.useGravity);

		ImGui::Separator();

		ImGui::DragFloat("Mass", &rb.mass, 0.02f, 0.01f, 1000.0f);
		ImGui::DragFloat("Restitution", &rb.restitution, 0.1f, 0.1f, 1000.0f);

		ImGui::Separator();

		ImGui::DragFloat("Static Friction", &rb.staticFriction, 0.05f, 0.1f, 10.0f);
		ImGui::DragFloat("Dynamic Friction", &rb.dynamicFriction, 0.05f, 0.1f, 10.0f);
		
		ImGui::Separator();

		ImGui::InputFloat3("Linear Velocity", (float*) &rb.linearVelocity);
		ImGui::InputFloat3("Angular Velocity", (float*) &rb.angularVelocity);
	}

	ImGui::End();
}

void PhysicsApplication::ShowSpawningMenu()
{
	ImGui::Begin("Spawning");

	SelectEntityType();

	ImGui::Separator();

	ImGui::Text("Entity Properties");

	Transform& transform = m_SpawnManager.GetProperties().transform;
	Rigidbody3D& rigidbody = m_SpawnManager.GetProperties().rigidbody;

	EntityTypes type = m_SpawnManager.GetSelectedEntityType();
	switch (type)
	{
	case EntityTypes::Cube:
		if (ImGui::CollapsingHeader("Transform")) {
			ImGui::DragFloat3("Scale", &transform.scale[0], 0.025f);
		}
		break;
	case EntityTypes::Sphere:
		if (ImGui::CollapsingHeader("Transform")) {
			if(ImGui::DragFloat("Radius", &m_Radius, 0.025f, 0.5f, 10.0f)) {
				transform.scale = glm::vec3(m_Radius);
			}
		}
		break;
	default:
		helpers::logError("PhysicsApplication::ShowSpawningMenu", "invalid entity type!");
	}

	if (ImGui::CollapsingHeader("Rigidbody")) {
		ImGui::Checkbox("Static", &rigidbody.isStatic);
		ImGui::Checkbox("Use Gravity", &rigidbody.useGravity);

		ImGui::Separator();

		ImGui::DragFloat("Mass", &rigidbody.mass, 0.02f, 0.01f, 1000.0f);
		ImGui::DragFloat("Restitution", &rigidbody.restitution, 0.1f, 0.1f, 1000.0f);

		ImGui::Separator();

		ImGui::DragFloat("Static Friction", &rigidbody.staticFriction, 0.05f, 0.1f, 10.0f);
		ImGui::DragFloat("Dynamic Friction", &rigidbody.dynamicFriction, 0.05f, 0.1f, 10.0f);
	}

	ImGui::Separator();

	ImGui::DragFloat3("Spawn Point", &m_SpawnManager.GetSpawnPointChangeable()[0], 0.1f);
	if (ImGui::Button("Spawn", ImVec2(-1, 0))) {
		Entity& entity = m_SpawnManager.Spawn(m_PhysicsWorld);

		if (m_PauseManager.GetCurrentState() == ApplicationStates::Play) {
			m_PhysicsWorld->AddToDeleteAfterStopSimulation(entity.GetId());
		}
	}

	ImGui::End();
}

void PhysicsApplication::SelectEntityType()
{
	const char* items[] = { "Cube", "Sphere" };

	int selectedType = (int)(m_SpawnManager.GetSelectedEntityType());
	if (ImGui::Combo("Select type", &selectedType, items, IM_ARRAYSIZE(items))) {
		m_SpawnManager.SetSelectedEntityType((EntityTypes)(selectedType));

		Transform& transform = m_SpawnManager.GetProperties().transform;
		transform.scale = glm::vec3(0.5f);
	}
}

#pragma once
#include <vector>
#include <map>

#include "Application.h"

#include "OpenGL/Shader.h"
#include "OpenGL/Camera.h"
#include "OpenGL/ShadowMap.h"

#include "Physics/PhysicsWorld.h"

//-----
#include "Objects/Axes.h"

#include "Managers/SpawnManager.h"

class PhysicsApplication : public Application
{
private:

	// Managers
	SpawnManager m_SpawnManager;

	// World
	PhysicsWorld m_PhysicsWorld;
	Entity* m_SelectedEntity = nullptr;
	std::unique_ptr<Axes> m_Axes;

	// OpenGL
	std::unique_ptr<Camera> m_Camera;
	std::unique_ptr<ShadowMap> m_ShadowMap;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_AxisShader;
	std::unique_ptr<Shader> m_ShadowShader;

	// Flags
	bool m_KeyPressed = false;
	bool m_LMButtonIsPressed = false;
	bool m_ShowAxes = false;

	// Light
	glm::vec3 m_LightPosition = glm::vec3(5.0f, 5.0f, 5.0f);
	glm::vec3 m_LightColor = glm::vec3(1);

	float m_NearPlane = 0.1f, m_FarPlane = 100.0f;
	glm::mat4 m_LightProjection = glm::mat4(1);
	glm::mat4 m_LightView = glm::mat4(1);

	// Temp
	Transform m_Transform1 = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(10.0f, 0.25f, 10.0f),
	};
	Rigidbody3D m_RB = {
		true,
		false,
		1.0f,
		1.0f
	};
	Transform m_Transform2 = {
		glm::vec3(0.0f, 2.5f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
	};

	EntityParameters m_Params1 = {
		m_Transform1,
		m_RB
	};

	EntityParameters m_Params2 = {
		m_Transform2,
		m_RB
	};

public:
	PhysicsApplication(){}
	~PhysicsApplication(){}

	void Start();
	void Update(float deltaTime);
	void Inputs(float deltaTime);
	
protected:
	void HandleOnSize(int width, int height);
	void HandleOnMouseMove(double xpos, double ypos);

	void ShowImGui();

private:
	void RenderScene();
	void RenderSceneDepthMap();
	void SelectEntityType();
};


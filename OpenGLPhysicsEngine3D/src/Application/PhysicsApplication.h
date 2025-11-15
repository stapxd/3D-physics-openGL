#pragma once
#include <vector>
#include <map>

#include "Application.h"
#include "OpenGL/Shader.h"

#include "OpenGL/Camera.h"

#include "Physics/PhysicsWorld.h"

//-----
#include "Objects/Cube.h"
#include "Objects/Axes.h"

#include "Structures/ObjectProperties.h"

#include "Managers/EntityManager.h"

class PhysicsApplication : public Application
{
private:

	std::unique_ptr<Camera> m_Camera;

	Entity* m_SelectedEntity = nullptr;
	bool m_LMButtonIsPressed = false;

	glm::vec3 m_SpawnPoint = glm::vec3(0);

	std::unique_ptr<Axes> m_Axes;

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_AxisShader;

	PhysicsWorld m_PhysicsWorld;

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
};


#pragma once
#include <array>

#include "Application.h"
#include "OpenGL/Shader.h"

#include "OpenGL/Camera.h"

#include "Physics/PhysicsWorld.h"

//-----
#include "Objects/Cube.h"
#include "Objects/Axes.h"

class PhysicsApplication : public Application
{
private:

	std::unique_ptr<Camera> m_Camera;
	bool m_MButtonIsPressed = false;

	std::vector<std::unique_ptr<IEntity>> m_Cubes;
	std::unique_ptr<Axes> m_Axes;


	glm::vec3 m_Scale = glm::vec3(1);
	glm::vec3 m_Rotation = glm::vec3(0);
	glm::vec3 m_Translation = glm::vec3(0);

	glm::vec3 m_Scale1 = glm::vec3(1);
	glm::vec3 m_Rotation1 = glm::vec3(0);
	glm::vec3 m_Translation1 = glm::vec3(0.0f, 0.0f, 10.0f);

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


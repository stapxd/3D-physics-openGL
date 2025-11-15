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
	bool m_MButtonIsPressed = false;

	EntityManager m_Manager;
	//std::map<std::unique_ptr<IEntity>, ObjectProperties> m_Objects;
	//std::vector<std::unique_ptr<IEntity>> m_Cubes;
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


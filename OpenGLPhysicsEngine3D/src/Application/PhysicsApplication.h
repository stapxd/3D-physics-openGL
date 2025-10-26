#pragma once
#include <array>

#include "Application.h"
#include "OpenGL/Shader.h"

#include "OpenGL/Camera.h"

#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndexBuffer.h"
#include "OpenGL/Shader.h"


//-----
#include "Objects/Cube.h"
#include "Objects/Axes.h"

class PhysicsApplication : public Application
{
private:

	std::unique_ptr<Camera> m_Camera;
	bool m_MButtonIsPressed = false;

	std::unique_ptr<Cube> m_Cube;
	std::unique_ptr<Axes> m_Axes;


	glm::vec3 m_Scale = glm::vec3(1);
	glm::vec3 m_Rotation = glm::vec3(0);
	glm::vec3 m_Translation = glm::vec3(0);

	std::unique_ptr<Shader> m_Shader;
	std::unique_ptr<Shader> m_AxisShader;


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


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

class PhysicsApplication : public Application
{
private:

	std::unique_ptr<Camera> m_Camera;
	bool m_MButtonIsPressed = false;

	std::unique_ptr<Cube> cube;
	float m_RotationAngle = 45.0f;

	std::unique_ptr<Shader> m_Shader;


public:
	PhysicsApplication(){}
	~PhysicsApplication(){}

	void Start();
	void Update(float deltaTime);
	void Inputs(float deltaTime);
	
protected:
	void HandleOnSize(int width, int height);
	void HandleOnMouseMove(double xpos, double ypos);
};


#include "PhysicsApplication.h"
#include <iostream>
#include <glm/glm.hpp>

#include "Renderer.h"

#include <vector>

void PhysicsApplication::Start()
{
	// Variables initialization
	m_Camera = std::make_unique<Camera>(m_Width, m_Height);
	
	cube = std::make_unique<Cube>();

	m_Shader = std::make_unique<Shader>("res/shaders/basic.shader");

	m_Shader->UnBind();
	
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.102f, 0.204f, 0.349f, 1.0f);
}

void PhysicsApplication::Update(float deltaTime)
{
    // Inputs
	m_Camera->Inputs(m_Window, deltaTime);
	
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("uProj", m_Camera->GetProjection());
	m_Shader->SetUniformMat4f("uView", m_Camera->GetView());
	
	glm::vec3 camPos = m_Camera->GetPosition();
	m_Shader->SetUniform3f("uCameraPosition", camPos.x, camPos.y, camPos.z);
	m_Shader->SetUniform3f("uColor", 0.5f, 0.5f, 0.5f);
	m_Shader->UnBind();

	//Render
	cube->Draw(*m_Shader);
}

void PhysicsApplication::Inputs(float deltaTime)
{
	// on_size() -> m_Camera->SetViewport(width, height);

#if false
	IEntity* entity = m_PhysicsWorld.GetEntity(0);

	float forceMagnitude = 5.0f;

	float dx = 0, dy = 0;

	if (glfwGetKey(m_Window, GLFW_KEY_W) == GLFW_PRESS) {
		dy = 1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_A) == GLFW_PRESS) {
		dx = -1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_S) == GLFW_PRESS) {
		dy = -1;
	}
	if (glfwGetKey(m_Window, GLFW_KEY_D) == GLFW_PRESS) {
		dx = 1;
	}

	if (dx != 0 || dy != 0) {
		glm::vec2 forceDirection = glm::normalize(glm::vec2(dx, dy));
		glm::vec2 force = forceDirection * forceMagnitude;
		entity->ApplyForce(force);
	}

	if (glfwGetKey(m_Window, GLFW_KEY_R) == GLFW_PRESS) {
		entity->Rotate(3.1415f / 8 * deltaTime);
	}
#endif
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

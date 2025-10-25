#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
	glm::vec3 m_Position = glm::vec3(-9.0f, 15.0f, -9.0f);
	glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 m_Direction = glm::vec3(0);

	// Mouse
	float m_CameraSensitivity = 0.05f;
	float m_Yaw = 45.0f;
	float m_Pitch = -45.0f;
	float m_LastX, m_LastY;
	bool m_MouseReleased = true;
	bool m_RightButtonPressed = false;

	int m_Width = 0, m_Height = 0;

	float m_MovementSpeed = 10.0f;

	float m_FOV = 75.0f;

public:
	Camera(int width, int height);

	void Inputs(GLFWwindow* window, float deltaTime);

	void Move(glm::vec3 vector);

	void SetViewport(int width, int height);

	glm::mat4 GetProjection() const;
	glm::mat4 GetView() const;
	glm::vec3 GetWorldPosition(glm::vec2 position) const; // attention
	glm::vec3 GetWorldPosition(double x, double y) const; // attention
	glm::vec3 GetPosition() const;

	void OnMouseMove(double xpos, double ypos);
	void EstimateDirection();
};


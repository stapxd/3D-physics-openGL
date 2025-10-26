#include "Camera.h"
#include <iostream>
Camera::Camera(int width, int height)
	: m_Width(width), m_Height(height)
{	
	m_LastX = width / 2.0f;
	m_LastY = height / 2.0f;

	EstimateDirection();
}

void Camera::Inputs(GLFWwindow* window, float deltaTime)
{	
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Move(m_MovementSpeed * m_Front * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Move(-1.0f * m_MovementSpeed * m_Front * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Move(-1.0f * glm::normalize(glm::cross(m_Front, m_Up)) * m_MovementSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Move(glm::normalize(glm::cross(m_Front, m_Up)) * m_MovementSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		Move(-m_Up * m_MovementSpeed * deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		Move(m_Up * m_MovementSpeed * deltaTime);
	}
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && m_MouseReleased) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_MouseReleased = false;
		m_RightButtonPressed = true;

		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		m_LastX = (float)xpos;
		m_LastY = (float)ypos;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		m_MouseReleased = true;
		m_RightButtonPressed = false;
	}

}

void Camera::Move(glm::vec3 vector)
{
	m_Position += vector;
	printf("campos: \nx: %.2f\ny: %.2f\nz: %.2f\n", m_Position.x, m_Position.y, m_Position.z);
}

void Camera::SetViewport(int width, int height)
{
	m_Width = glm::max(10, width);
	m_Height = glm::max(10, height);
}

glm::mat4 Camera::GetProjection() const
{
	////2D:
	//float halfW = (m_Width / 2.0f)  / m_Zoom;
	//float halfH = (m_Height / 2.0f) / m_Zoom;
	//return glm::ortho(-halfW, halfW, -halfH, halfH);

	//3D
	return glm::perspective(m_FOV, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);
}

glm::mat4 Camera::GetView() const
{
	return glm::lookAt(m_Position, m_Position + m_Front, -m_Up);
}

glm::vec3 Camera::GetWorldPosition(glm::vec2 position) const
{
	glm::vec3 screenPos(position.x, m_Height - position.y, 0.0f);
	glm::vec4 viewport(0.0f, 0.0f, m_Width, m_Height);

	return glm::unProject(screenPos, GetView(), GetProjection(), viewport);
}

glm::vec3 Camera::GetWorldPosition(double x, double y) const
{
	glm::vec3 screenPos(x, m_Height - y, 0.0f);
	glm::vec4 viewport(0.0f, 0.0f, m_Width, m_Height);

	return glm::unProject(screenPos, GetView(), GetProjection(), viewport);
}

glm::vec3 Camera::GetPosition() const
{
	return m_Position;
}

void Camera::OnMouseMove(double xpos, double ypos)
{
	if (!m_RightButtonPressed) return;

	float xoffset = xpos - m_LastX;
	float yoffset = m_LastY - ypos;
	m_LastX = (float)xpos;
	m_LastY = (float)ypos;

	xoffset *= m_CameraSensitivity;
	yoffset *= m_CameraSensitivity;

	m_Yaw += xoffset;
	m_Pitch += yoffset;

	m_Yaw = fmod(m_Yaw, 360.0f);
	if (m_Yaw < 0.0f)
		m_Yaw += 360.0f;

	m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);

	//printf("yaw: %.2f\n pitch: %.2f\n", m_Yaw, m_Pitch);

	EstimateDirection();
}

void Camera::EstimateDirection()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	direction.y = sin(glm::radians(m_Pitch));
	direction.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_Front = glm::normalize(direction);
}

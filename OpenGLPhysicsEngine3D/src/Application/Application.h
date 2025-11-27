#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Application
{
protected:
	GLFWwindow* m_Window = nullptr;

	int m_Width = 0, m_Height = 0;

public:
	Application();
	virtual ~Application();

	bool Init(int width, int height, const char* title);
	void Run();

	virtual void Start();
	virtual void Update(float deltaTime);
	virtual void Inputs(float deltaTime);

	explicit operator bool();
public:
	// Getters
	int GetWidth() { return m_Width; }
	int GetHeight() { return m_Height; }

	void GetCursorPosition(double* xPos, double* yPos);

protected:
	// Handling inputs
	virtual void HandleOnSize(int width, int height);
	virtual void HandleOnMouseMove(double xpos, double ypos);

	//ImGui
	virtual void ShowImGui();

private:
	static void OnSize(GLFWwindow* window, int width, int height);
	static void OnMouseMove(GLFWwindow* window, double xpos, double ypos);

private:
	void InitCallbacks();

private:
	float m_DeltaTime = 0.0f;
	float m_LastFrame = 0.0f;
};


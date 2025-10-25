#include "Application.h"

#include <iostream>

Application::Application()
{
    
}

Application::~Application()
{
    glfwTerminate();
}

bool Application::Init(int width, int height, const char* title)
{
    if (!glfwInit()) {
        std::cout << "Unable to init GLFW." << "\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Width = width;
    m_Height = height;

    m_Window = glfwCreateWindow(m_Width, m_Height, title, NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "Unable to create window." << "\n";
        return false;
    }

    glfwMakeContextCurrent(m_Window);

    //glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Unable to init GLEW." << "\n";
        return false;
    }

    InitCallbacks();

    return true;
}

void Application::InitCallbacks()
{
    glfwSetWindowSizeCallback(m_Window, OnSize);
    glfwSetCursorPosCallback(m_Window, OnMouseMove);
    glfwSetWindowUserPointer(m_Window, this);
}

void Application::OnSize(GLFWwindow* window, int width, int height)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->HandleOnSize(width, height);
    }
}

void Application::OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
    Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        app->HandleOnMouseMove(xpos, ypos);
    }
}

void Application::Run()
{
    Start();

    while (!glfwWindowShouldClose(m_Window))
    {
        float currentFrame = (float)glfwGetTime();
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Inputs(m_DeltaTime);
        Update(m_DeltaTime);

        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }
}

void Application::Start()
{
}

void Application::Update(float deltaTime)
{
}

void Application::Inputs(float deltaTime)
{
}

Application::operator bool()
{
	return m_Window != nullptr;
}

void Application::HandleOnSize(int width, int height)
{
}

void Application::HandleOnMouseMove(double xpos, double ypos)
{
}

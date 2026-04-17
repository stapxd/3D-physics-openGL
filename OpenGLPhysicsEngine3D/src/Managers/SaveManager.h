#pragma once

#include "Physics/PhysicsWorld.h"

#include <iostream>

enum class SaveActions {
	Save,
	Load
};

class SaveManager
{
public:
	SaveManager(PhysicsWorld& physicsWorld);

	void Save(GLFWwindow* window);
	void Load(GLFWwindow* window);

	std::string GetFilePathWithExplorer(SaveActions action, GLFWwindow* window);

private:
	PhysicsWorld& m_PhysicsWorld;
};


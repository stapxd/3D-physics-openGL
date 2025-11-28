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

	void Save();
	void Load();

	std::string GetFilePathWithExplorer(SaveActions action);

private:
	PhysicsWorld& m_PhysicsWorld;
};


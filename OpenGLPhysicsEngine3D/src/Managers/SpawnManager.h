#pragma once
#include <glm/glm.hpp>

#include "Physics/PhysicsWorld.h"
#include "Enumerators/EntityTypes.h"
#include "Structures/EntityParameters.h"

class SpawnManager
{
public:
	SpawnManager();

	void Spawn(PhysicsWorld& physicsWorld);

	glm::vec3& GetSpawnPointChangeable() { return m_SpawnPoint; }

	void SetSelectedEntityType(EntityTypes type) { m_SelectedType = type; }
	EntityTypes GetSelectedEntityType() { return m_SelectedType; }

	EntityParameters& GetParams() { return m_Params; }

private:
	glm::vec3 m_SpawnPoint = glm::vec3(0);
	EntityTypes m_SelectedType = EntityTypes::Cube;
	EntityParameters m_Params;
};


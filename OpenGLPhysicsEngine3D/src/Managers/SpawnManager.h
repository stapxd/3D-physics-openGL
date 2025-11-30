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

	ObjectProperties& GetProperties() { return m_Properties; }

private:
	glm::vec3 m_SpawnPoint = glm::vec3(0);
	EntityTypes m_SelectedType = EntityTypes::Cube;
	ObjectProperties m_Properties;
};


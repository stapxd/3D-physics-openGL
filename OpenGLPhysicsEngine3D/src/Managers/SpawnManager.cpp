#include "SpawnManager.h"

SpawnManager::SpawnManager()
{
}

void SpawnManager::Spawn(PhysicsWorld& physicsWorld)
{
	Entity& entity = physicsWorld->AddEntity(m_SelectedType, m_Params);
	entity.GetProperties().transform.translation = m_SpawnPoint;
}

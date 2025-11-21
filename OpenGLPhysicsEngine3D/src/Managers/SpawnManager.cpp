#include "SpawnManager.h"

SpawnManager::SpawnManager()
{
	m_Params.rigidbody.mass = 1.0f;
	m_Params.rigidbody.restitution = 0.5f;
}

void SpawnManager::Spawn(PhysicsWorld& physicsWorld)
{
	Entity& entity = physicsWorld->AddEntity(m_SelectedType, m_Params);
	entity.GetProperties().transform.translation = m_SpawnPoint;
}

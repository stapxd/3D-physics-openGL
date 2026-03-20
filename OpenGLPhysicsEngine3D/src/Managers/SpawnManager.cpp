#include "SpawnManager.h"

SpawnManager::SpawnManager()
{
	m_Properties.rigidbody.mass = 1.0f;
	m_Properties.rigidbody.restitution = 0.5f;
}

Entity& SpawnManager::Spawn(PhysicsWorld& physicsWorld)
{
	Entity& entity = physicsWorld->AddEntity(m_SelectedType, m_Properties);
	entity.GetProperties().transform.translation = m_SpawnPoint;
	return entity;
}

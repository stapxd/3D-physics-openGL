#include "EntityManager.h"

#include "Factories/EntityFactory.h"

#include "Application/Globals.h"

Entity& EntityManager::AddEntity(EntityTypes type, const ObjectProperties& properties)
{
	std::lock_guard<std::mutex> lock(Globals::s_EntitySpawnMutex);

	auto entityPtr = EntityFactory::CreateEntity(type);
	Entity entity(m_Size, type, std::move(entityPtr));
	entity.GetProperties() = properties;

	m_Entities[m_Size] = std::move(entity);

	m_Size++;
	return m_Entities[m_Size - 1];
}

Entity& EntityManager::AddEntity(Entity& entity)
{
	std::lock_guard<std::mutex> lock(Globals::s_EntitySpawnMutex);
	entity.SetId(m_Size);
	m_Entities[m_Size] = std::move(entity);

	m_Size++;
	return m_Entities[m_Size - 1];
}

Entity& EntityManager::FindEntity(unsigned int id)
{
	if (id < 0 || id > m_Size)
		throw std::out_of_range("EntityManager::FindEntity : id is out of range");
	return m_Entities[id];
}

void EntityManager::ClearAll()
{
	std::lock_guard<std::mutex> lock(Globals::s_EntitySpawnMutex);
	m_Size = 0;
	m_Entities.clear();
}

void EntityManager::SetEntityPropertiesFromParameters(Entity& entity, const ObjectProperties& params)
{
	// Transform
	entity.GetProperties().transform.scale = params.transform.scale;

	// Rigidbody
	entity.GetProperties().rigidbody.isStatic = params.rigidbody.isStatic;
	entity.GetProperties().rigidbody.useGravity = params.rigidbody.useGravity;
	entity.GetProperties().rigidbody.mass = params.rigidbody.mass;
	entity.GetProperties().rigidbody.restitution = params.rigidbody.restitution;

	entity->EstimateInertiaTensor(entity.GetProperties().rigidbody);
}

#include "EntityManager.h"

#include "Factories/EntityFactory.h"

#include "Application/Globals.h"

Entity& EntityManager::AddEntity(EntityTypes type, const ObjectProperties& properties)
{
	auto entityPtr = EntityFactory::CreateEntity(type);
	Entity entity(m_LastId, type, std::move(entityPtr));
	entity.GetProperties() = properties;

	m_Entities[m_LastId] = std::move(entity);

	m_LastId++;
	return m_Entities[m_LastId - 1];
}

Entity& EntityManager::AddEntity(Entity& entity)
{
	entity.SetId(m_LastId);
	m_Entities[m_LastId] = std::move(entity);

	m_LastId++;
	return m_Entities[m_LastId - 1];
}

void EntityManager::Flush()
{
	for (auto id : m_ToDelete) {
		if (FindEntity(id) != nullptr)
			m_Entities.erase(id);
	}
	m_ToDelete.clear();
}

void EntityManager::DeleteEntity(unsigned int id)
{
	if (id < 0 || id >= m_LastId)
		throw std::out_of_range("EntityManager::FindEntity : id is out of range");
	m_ToDelete.push_back(id);
}

Entity* EntityManager::FindEntity(unsigned int id)
{
	if (m_Entities.find(id) != m_Entities.end())
		return &m_Entities[id];
	else
		return nullptr;
}

void EntityManager::ClearAll()
{
	m_LastId = 0;
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

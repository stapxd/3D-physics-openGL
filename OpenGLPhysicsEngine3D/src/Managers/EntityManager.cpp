#include "EntityManager.h"

#include "Factories/EntityFactory.h"

Entity& EntityManager::AddEntity(EntityTypes type, const EntityParameters& params)
{
	auto entityPtr = EntityFactory::CreateEntity(type);
	Entity entity(m_Size, std::move(entityPtr));
	m_Entities[m_Size] = std::move(entity);

	SetEntityPropertiesFromParameters(m_Entities[m_Size], params);

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
	m_Size = 0;
	m_Entities.clear();
}

void EntityManager::SetEntityPropertiesFromParameters(Entity& entity, const EntityParameters& params)
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

#include "EntityManager.h"

#include "Factories/EntityFactory.h"

Entity& EntityManager::AddEntity(EntityTypes type, const EntityParameters& params)
{
	auto entityPtr = EntityFactory::CreateEntity(type, params);
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

void EntityManager::SetEntityPropertiesFromParameters(Entity& entity, const EntityParameters& params)
{
	// Maybe unite both Properties and PhysicsBody, so we dont need to parse it to both
	entity.GetProperties().physicsProperties.isStatic = params.isStatic;
	entity->GetPhysicsBody()->SetStatic(params.isStatic);
}

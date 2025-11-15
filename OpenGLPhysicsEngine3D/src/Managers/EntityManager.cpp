#include "EntityManager.h"

#include "Factories/EntityFactory.h"

void EntityManager::AddEntity(EntityTypes type, const EntityParameters& params)
{
	auto entityPtr = EntityFactory::CreateEntity(type, params);
	Entity entity(std::move(entityPtr));
	m_Entities[m_Size] = std::move(entity);
	m_Size++;
}

Entity& EntityManager::FindEntity(unsigned int id)
{
	if (id < 0 || id > m_Size)
		throw std::out_of_range("EntityManager::FindEntity : id is out of range");
	return m_Entities[id];
}

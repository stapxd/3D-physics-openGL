#pragma once
#include <unordered_map>

#include "Objects/Entity.h"

#include "Structures/ObjectProperties.h"

class EntityManager
{
public:
	Entity& AddEntity(EntityTypes type, const ObjectProperties& params);
	Entity& AddEntity(Entity& entity);
	Entity& FindEntity(unsigned int id);
	void ClearAll();

	void SetEntityPropertiesFromParameters(Entity& entity, const ObjectProperties& params);

	std::unordered_map<unsigned int, Entity>& GetEntities() { return m_Entities; }
	const std::unordered_map<unsigned int, Entity>& GetEntities() const { return m_Entities; }

	unsigned int GetSize() const { return m_Size; }
private:
	unsigned int m_Size = 0;
	std::unordered_map<unsigned int, Entity> m_Entities;
};

